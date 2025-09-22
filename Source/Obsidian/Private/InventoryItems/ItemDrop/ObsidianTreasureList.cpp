// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemDrop/ObsidianTreasureList.h"

// ~ Core
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // ~ With Editor

// ~ Project
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "ObsidianTypes/ObsidianItemTypes.h"

// ~ FObsidianDropItem

const FObsidianDropItem FObsidianDropItem::NoDropType;

FObsidianDropItem::FObsidianDropItem()
{
	if (StackSizes.IsEmpty())
	{
		StackSizes.AddDefaulted();
	}
}

bool FObsidianDropItem::IsValid() const
{
	return !TreasureItemDefinitionClass.IsNull();
}

uint8 FObsidianDropItem::GetRandomStackSizeToDropAdjusted(const uint8 TreasureQuality) const
{
	TArray<FObsidianStacksToDrop> AdjustedStackSizes;
	AdjustedStackSizes.Reserve(StackSizes.Num());
	AdjustedStackSizes.Append(StackSizes);
	
	uint8 MinStack = 255;
	uint8 MaxStack = 0;
	for (const FObsidianStacksToDrop& StackSizeConfig : AdjustedStackSizes)
	{
		MinStack = FMath::Min(MinStack, StackSizeConfig.StackSize);
		MaxStack = FMath::Max(MaxStack, StackSizeConfig.StackSize);
	}

	const float RollBias = TreasureQuality / ObsidianTreasureStatics::MaxTreasureQuality;
	uint32 TotalAdjustedWeight = 0;
	for (FObsidianStacksToDrop& AdjustedStackSizeConfig : AdjustedStackSizes)
	{
		float StackNorm = (MaxStack > MinStack) ? (static_cast<float>(AdjustedStackSizeConfig.StackSize) - MinStack) / (MaxStack - MinStack) : 1.0f;
		AdjustedStackSizeConfig.DropWeight = AdjustedStackSizeConfig.DropWeight * FMath::Lerp(1.0f - RollBias, 1.0f + RollBias, StackNorm);
		TotalAdjustedWeight += AdjustedStackSizeConfig.DropWeight;
	}
	
	const uint32 Roll = FMath::RandRange(0, TotalAdjustedWeight);
	uint32 Cumulative = 0;
	
	for (const FObsidianStacksToDrop& AdjustedStackSizeConfig : AdjustedStackSizes)
	{
		Cumulative += AdjustedStackSizeConfig.DropWeight;
		if (Roll <= Cumulative)
		{
			return AdjustedStackSizeConfig.StackSize;
		}
	}
	
	return 1;
}

// ~ FObsidianTreasureClass

FObsidianDropItem FObsidianTreasureClass::GetRandomItemFromClass(const float NoDropScale)
{
	const uint16 ScaledNoDropWeight = FMath::Max((NoDropWeight * NoDropScale), 0);
	uint32 TotalWeight = ScaledNoDropWeight;
	for (const FObsidianDropItem& DropItem : DropItems)
	{
		TotalWeight += DropItem.DropWeight;
	}

	const uint32 Roll = FMath::RandRange(0, TotalWeight);
	if (Roll <= ScaledNoDropWeight)
	{
		return FObsidianDropItem::NoDropType;
	}
	
	uint32 Cumulative = 0;
	for (const FObsidianDropItem& DropItem : DropItems)
	{
		Cumulative += DropItem.DropWeight;
		if (Roll <= (ScaledNoDropWeight + Cumulative))
		{
			return DropItem;
		}
	}

	return FObsidianDropItem::NoDropType;
}

#if WITH_EDITOR
EDataValidationResult UObsidianTreasureList::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	uint16 TreasureClassesIndex = 0;
	for (const FObsidianTreasureClass& Class : TreasureClasses)
	{
		Result =  CombineDataValidationResults(Result, Class.ValidateData(Context, TreasureClassesIndex));
		TreasureClassesIndex++;
	}
	
	return Result;
}
#endif

// ~ End of FObsidianTreasureClass

void UObsidianTreasureList::PostInitProperties()
{
	Super::PostInitProperties();

	TreasureClasses.Sort([](const FObsidianTreasureClass& A, const FObsidianTreasureClass& B)
		{
			return A.TreasureQuality < B.TreasureQuality;
		});

	for (FObsidianTreasureClass& TreasureClass : TreasureClasses)
	{
		for (FObsidianDropItem& DropItem : TreasureClass.DropItems)
		{
			DropItem.StackSizes.Sort([](const FObsidianStacksToDrop& A, const FObsidianStacksToDrop& B)
				{
					return A.DropWeight > B.DropWeight;
				});
		}
	}
}

void UObsidianTreasureList::PostLoad()
{
	Super::PostLoad();
	
	TreasureClassMap.Empty(TreasureClasses.Num());
	for (const FObsidianTreasureClass& Class : TreasureClasses)
	{
		TreasureClassMap.Add(Class.TreasureQuality, &Class);
	}
}

TArray<FObsidianTreasureClass> UObsidianTreasureList::GetAllTreasureClasses() const
{
	return TreasureClasses;
}

TArray<FObsidianTreasureClass> UObsidianTreasureList::GetAllTreasureClassesUpToQuality(const uint8 TreasureQuality)
{
	TArray<FObsidianTreasureClass> MatchingTreasureClasses;
	
	for (const FObsidianTreasureClass& Class : TreasureClasses)
	{
		if (Class.TreasureQuality <= TreasureQuality)
		{
			MatchingTreasureClasses.Add(Class);
		}
	}

	return MatchingTreasureClasses;
}

const FObsidianTreasureClass* UObsidianTreasureList::GetTreasureClassOfQuality(const uint8 TreasureQuality)
{
	if (const FObsidianTreasureClass** TreasureClassPtr = TreasureClassMap.Find(TreasureQuality))
	{
		return *TreasureClassPtr;
	}
	return nullptr;
}

#if WITH_EDITOR
EDataValidationResult FObsidianTreasureClass::ValidateData(FDataValidationContext& Context, const int Index) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	if(TreasureQuality == 0)
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Treasure Quality at index [%i] is not set! \n"
			"Please set a valid Treasure Quality or delete this index entry in this Treasure Class."), Index));

		Context.AddError(ErrorMessage);
	}
	
	if(DropItems.IsEmpty())
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Drop Items at index [%i] is empty! \n"
			"Please fill Drop Items or delete this index entry in this Treasure Class."), Index));

		Context.AddError(ErrorMessage);
	}

	for (int32 i = 0; i < DropItems.Num(); i++)
	{
		const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef = DropItems[i].TreasureItemDefinitionClass.LoadSynchronous();
		if (ItemDef == nullptr)
		{
			Result = EDataValidationResult::Invalid;

			const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Item Def at Drop Items index [%i] at overall index [%i] is empty! \n"
				"Please fill correct Treasure Item Definition Class or delete this index entry."), i, Index));

			Context.AddError(ErrorMessage);
		}
	}

	return Result;
}
#endif


// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemDrop/ObsidianTreasureList.h"

// ~ Core
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // ~ With Editor

// ~ Project
#include "InventoryItems/ObsidianInventoryItemDefinition.h"

// ~ FObsidianTreasureClass

TSoftClassPtr<UObsidianInventoryItemDefinition> FObsidianTreasureClass::GetRandomItemFromClass(const float NoDropScale)
{
	const int32 ScaledNoDropWeight = NoDropWeight * NoDropScale;
	int32 TotalWeight = ScaledNoDropWeight;
	for (const FObsidianDropItem& DropItem : DropItems)
	{
		TotalWeight += DropItem.DropWeight;
	}

	const int32 Roll = FMath::RandRange(0, TotalWeight);
	if (Roll <= ScaledNoDropWeight)
	{
		UE_LOG(LogTemp, Display, TEXT("No Drop Weight was rolled."));
		return nullptr;
	}
	
	int32 Cumulative = 0;
	for (const FObsidianDropItem& DropItem : DropItems)
	{
		Cumulative += DropItem.DropWeight;
		if (Roll <= (ScaledNoDropWeight + Cumulative))
		{
			return DropItem.TreasureItemDefinitionClass;
		}
	}

	return nullptr;
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

void UObsidianTreasureList::PostLoad()
{
	Super::PostLoad();
	
	TreasureClasses.Sort([](const FObsidianTreasureClass& A, const FObsidianTreasureClass& B)
		{
			return A.TreasureQuality < B.TreasureQuality;
		});

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

TArray<FObsidianTreasureClass> UObsidianTreasureList::GetAllTreasureClassesUpToQuality(const int32 TreasureQuality)
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

const FObsidianTreasureClass* UObsidianTreasureList::GetTreasureClassOfQuality(const int32 TreasureQuality)
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


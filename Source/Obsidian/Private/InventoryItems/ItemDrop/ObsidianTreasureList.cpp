// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemDrop/ObsidianTreasureList.h"

// ~ Core
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // ~ With Editor

// ~ Project
#include "InventoryItems/ObsidianInventoryItemDefinition.h"

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

bool UObsidianTreasureList::ShouldAlwaysRoll() const
{
	return bShouldAlwaysRoll;
}

void UObsidianTreasureList::SetShouldAlwaysRoll(const bool InbShouldAlwaysRoll)
{
	bShouldAlwaysRoll = InbShouldAlwaysRoll;
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

EDataValidationResult UObsidianTreasureList::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	unsigned int TreasureClassesIndex = 0;
	for(const FObsidianTreasureClass& Class : TreasureClasses)
	{
		Result =  CombineDataValidationResults(Result, Class.ValidateData(Context, TreasureClassesIndex));
		TreasureClassesIndex++;
	}
	
	return Result;
}
#endif

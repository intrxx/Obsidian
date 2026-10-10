// Copyright 2026 out of sCope team - intrxx

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "Core/ObsidianGameplayStatics.h"
#include "InventoryItems/ItemAffixes/ObsidianItemAffixStack.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "Obsidian/ObsidianLogCategories.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif


// ~ FObsidianItemGeneratedData

void FObsidianItemGeneratedData::SetStackCount(const int32 InStackCount)
{
	AvailableStackCount = InStackCount;
}

int32 FObsidianItemGeneratedData::GetStackCount() const
{
	return AvailableStackCount;
}

void FObsidianItemGeneratedData::Reset()
{
	AvailableStackCount = 1;
	ItemLevel = 1;
	ItemRarity = EObsidianItemRarity::None;
	ItemAffixes.Reset();
	NameData = FObsidianItemGeneratedNameData();
	ItemEquippingRequirements = FObsidianItemRequirements();
}

// ~ FDraggedItem

FDraggedItem::FDraggedItem(UObsidianInventoryItemInstance* InInstance) 
	: Instance(InInstance)
	, GeneratedData(InInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current))
{}

bool FDraggedItem::IsEmpty() const
{
	return !Instance && !ItemDef;
}

bool FDraggedItem::CarriesItemDef() const
{
	return ItemDef != nullptr;
}

// ~ FObsidianSlotDefinition

FObsidianSlotDefinition const FObsidianSlotDefinition::InvalidSlot;

// ~ FObsidianItemRequirement

FObsidianItemRequirements::FObsidianItemRequirements()
	: bHasAnyRequirements(false)
	, bInitialized(false)
{
	AttributeRequirements =
		{
			{FObsidianAttributeRequirement(UObsidianHeroAttributeSet::GetStrengthAttribute(), 0)},
			{FObsidianAttributeRequirement(UObsidianHeroAttributeSet::GetIntelligenceAttribute(), 0)},
			{FObsidianAttributeRequirement(UObsidianHeroAttributeSet::GetDexterityAttribute(), 0)},
			{FObsidianAttributeRequirement(UObsidianHeroAttributeSet::GetFaithAttribute(), 0)}
		};
}

// ~ FObsidianItemRequirement

bool FObsidianSlotDefinition::IsValid() const
{
	return SlotTag.IsValid();
}

FGameplayTag FObsidianSlotDefinition::GetSlotTag() const
{
	return SlotTag;
}

EObsidianPlacingAtSlotResult FObsidianSlotDefinition::CanPlaceAtSlot(const FGameplayTag& InItemCategory) const
{
	if(BannedItemCategories.HasTagExact(InItemCategory))
	{
		return EObsidianPlacingAtSlotResult::UnableToPlace_BannedCategory;
	}
	
	if(AcceptedItemCategories.HasTagExact(InItemCategory))
	{
		return EObsidianPlacingAtSlotResult::CanPlace;
	}
	
	return EObsidianPlacingAtSlotResult::UnableToPlace_UnfitForCategory;
}

void FObsidianSlotDefinition::AddBannedItemCategory(const FGameplayTag& InBannedCategory)
{
	BannedItemCategories.AddTag(InBannedCategory);
}

void FObsidianSlotDefinition::AddBannedItemCategories(const FGameplayTagContainer& InBannedCategories)
{
	BannedItemCategories.AppendTags(InBannedCategories);
}

void FObsidianSlotDefinition::RemoveBannedItemCategory(const FGameplayTag& InBannedCategoryToRemove)
{
#if !UE_BUILD_SHIPPING
	if(BannedItemCategories.HasTag(InBannedCategoryToRemove) == false)
	{
		UE_LOG(ObLogItems, Error, TEXT("Trying to remove Banned Equipment Tag [%s] but the Tag does not exist"
							  " in BannedItemCategories."), *InBannedCategoryToRemove.ToString());
	}
#endif
	BannedItemCategories.RemoveTag(InBannedCategoryToRemove);
}

void FObsidianSlotDefinition::RemoveBannedItemCategories(const FGameplayTagContainer& InBannedCategoriesToRemove)
{
#if !UE_BUILD_SHIPPING
	for(FGameplayTag Tag : BannedItemCategories)
	{
		if(BannedItemCategories.HasTag(Tag) == false)
		{
			UE_LOG(ObLogItems, Error, TEXT("Trying to remove Banned Equipment Tag [%s] but the Tag does not exist"
							   " in BannedItemCategories."), *Tag.ToString());
		}
	}
#endif
	BannedItemCategories.RemoveTags(InBannedCategoriesToRemove);
}

// ~ FObsidianItemPosition

bool FObsidianItemPosition::IsValid() const
{
	return (Type != EObsidianItemPositionType::None)
		&& ((GridPosition != FIntPoint::NoneValue) || (SlotTag != FGameplayTag::EmptyTag));
}

bool FObsidianItemPosition::IsOnInventoryGrid() const
{
	return (Type == EObsidianItemPositionType::InventoryGrid) && (GridPosition != FIntPoint::NoneValue);
}

bool FObsidianItemPosition::IsOnEquipmentSlot() const
{
	return (Type == EObsidianItemPositionType::EquipmentSlot) && (SlotTag != FGameplayTag::EmptyTag);
}

bool FObsidianItemPosition::IsOnStash() const
{
	return IsOnStashGrid() || IsOnStashSlot();
}

bool FObsidianItemPosition::IsOnStashGrid() const
{
	return (Type == EObsidianItemPositionType::StashGrid) && (GridPosition != FIntPoint::NoneValue);
}

bool FObsidianItemPosition::IsOnStashSlot() const
{
	return (Type == EObsidianItemPositionType::StashSlot) && (SlotTag != FGameplayTag::EmptyTag);
}

void FObsidianItemPosition::Reset()
{
	Type = EObsidianItemPositionType::None;
	GridPosition = FIntPoint::NoneValue;
	SlotTag = FGameplayTag::EmptyTag;
	OwningStashTabTag = FGameplayTag::EmptyTag;
}

FIntPoint FObsidianItemPosition::GetItemGridPosition(const bool bInWarnIfNotFound) const
{
#if !UE_BUILD_SHIPPING
	if(bInWarnIfNotFound && GridPosition == FIntPoint::NoneValue)
	{
		UE_LOG(ObLogItems, Error, TEXT("Grid Location is invalid in [%hs]."), __FUNCTION__);
	}
#endif
	return GridPosition;
}

FGameplayTag FObsidianItemPosition::GetItemSlotTag(const bool bInWarnIfNotFound) const
{
#if !UE_BUILD_SHIPPING
	if(bInWarnIfNotFound && SlotTag == FGameplayTag::EmptyTag)
	{
		UE_LOG(ObLogItems, Error, TEXT("Slot Tag is invalid in [%hs]."), __FUNCTION__);
	}
#endif
	return SlotTag;
}

FGameplayTag FObsidianItemPosition::GetOwningStashTabTag() const
{
	return OwningStashTabTag;
}

FString FObsidianItemPosition::GetDebugStringPosition() const
{
	if (OwningStashTabTag != FGameplayTag::EmptyTag)
	{
		if (SlotTag != FGameplayTag::EmptyTag)
		{
			return FString::Printf(TEXT("Stash Tab: [%s], Slot: [%s]"),
				*OwningStashTabTag.GetTagName().ToString(), *SlotTag.GetTagName().ToString());
		}
		if (GridPosition != FIntPoint::NoneValue)
		{
			return FString::Printf(TEXT("Stash Tab: [%s], Grid Location: [%d, %d]"),
				*OwningStashTabTag.GetTagName().ToString(), GridPosition.X, GridPosition.Y);
		}
	}
	else
	{
		if (SlotTag != FGameplayTag::EmptyTag)
		{
			return FString::Printf(TEXT("Equipment Slot: [%s]"), *SlotTag.GetTagName().ToString());
		}
		if (GridPosition != FIntPoint::NoneValue)
		{
			return FString::Printf(TEXT("Inventory Grid Location: [%d, %d]"), GridPosition.X, GridPosition.Y);
		}
	}

	return FString::Printf(TEXT("Error: Position of an item is not initialized correctly!"));
}

// ~ FObsidianAffixValues

bool FObsidianAffixValues::IsValid() const
{
	return AffixValuesIdentifiers.IsEmpty() == false && PossibleAffixRanges.IsEmpty() == false;
}

// ~ FObsidianActiveAffixValue

bool FObsidianActiveAffixValue::IsValid() const
{
	return !AffixValues.IsEmpty();
}

// ~ FObsidianStaticItemAffix

bool FObsidianStaticItemAffix::IsEmptyImplicit() const
{
	return AffixType == EObsidianAffixType::Implicit && IsEmptyAffix();
}

bool FObsidianStaticItemAffix::IsEmptyAffix() const
{
	return AffixValuesDefinition.IsValid();
}

FObsidianStaticItemAffix::operator bool() const
{
	return AffixTag.IsValid();
}

bool FObsidianStaticItemAffix::operator==(const FObsidianStaticItemAffix& InOther) const
{
	return AffixTag == InOther.AffixTag;
}

bool FObsidianStaticItemAffix::operator==(const FObsidianDynamicItemAffix& InOther) const
{
	return AffixTag == InOther.AffixTag;
}

bool FObsidianStaticItemAffix::operator==(const FObsidianActiveItemAffix& InOther) const
{
	return AffixTag == InOther.AffixTag;
}

#if WITH_EDITOR
EDataValidationResult FObsidianStaticItemAffix::IsStaticAffixValid(FDataValidationContext& InContext, const int32 InIndex,
	const FString& InAffixTypeName) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;
	
	if (AffixTag.IsValid() == false || AffixTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Item.Affix"))) == false)
	{
		Result = EDataValidationResult::Invalid;
			
		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Affix Tag at index [%i] of [%s] Affix"
			" is invalid! \n Please fill correct Affix Tag."), InIndex, *InAffixTypeName));
		InContext.AddError(ErrorMessage);
	}
		
	if (bOverride_AffixAbilitySet && SoftAbilitySetToApply.IsNull())
	{
		Result = EDataValidationResult::Invalid;
		
		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("SoftAbilitySetToApply at index [%i] of"
			" [%s] Affix is not set! \n Please provide a valid AbilitySet to apply!"), InIndex, *InAffixTypeName));
		InContext.AddError(ErrorMessage);
	}
	else if (bOverride_AffixAbilitySet == false && SoftAbilitySetToApply.IsNull() == false)
	{
		Result = EDataValidationResult::Invalid;
		
		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("SoftAbilitySetToApply at index [%i] of"
			" [%s] Affix is set but the Affix does not Override it! \n Please re-check the asset!"), InIndex, *InAffixTypeName));
		InContext.AddError(ErrorMessage);
	}

	if (AffixValuesDefinition.IsValid() == false)
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("PossibleAffixRanges at index [%i] of"
			" [%s] Affix are not set! \n Please fill it with possible affix ranges."), InIndex, *InAffixTypeName));
		InContext.AddError(ErrorMessage);
	}
		
	uint8 ExpectedCount = AffixValuesDefinition.AffixValuesIdentifiers.Num();
	for (int32 x = 0; x < ExpectedCount; x++)
	{
		if (AffixValuesDefinition.AffixValuesIdentifiers[x].AffixValueID.IsValid() == false)
		{
			Result = EDataValidationResult::Invalid;

			const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("AffixValueID at index [%i] inside"
				" [%s] Affix at index [%i] of AffixValuesIdentifiers is not set! \n Please make sure to fill the"
				" AffixValueID tag."),x, *InAffixTypeName, InIndex));
			InContext.AddError(ErrorMessage);
		}
			
		if (AffixValuesDefinition.AffixValuesIdentifiers[x].bOverride_AttributeToModify &&
			AffixValuesDefinition.AffixValuesIdentifiers[x].AttributeToModify.IsValid() == false)
		{
			Result = EDataValidationResult::Invalid;

			const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("AttributeToModify at index [%i]"
				" inside [%s] Affix at index [%i] of AffixValuesIdentifiers is not set! \n Please make sure to either"
				" correct the bOverride_AttributeToModify or fill the Attribute."),x, *InAffixTypeName, InIndex));
			InContext.AddError(ErrorMessage);
		}
		else if (AffixValuesDefinition.AffixValuesIdentifiers[x].bOverride_AttributeToModify == false &&
			AffixValuesDefinition.AffixValuesIdentifiers[x].AttributeToModify.IsValid())
		{
			Result = EDataValidationResult::Invalid;

			const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("AttributeToModify at index [%i]"
				" inside [%s] Affix at index [%i] of AffixValuesIdentifiers is set but the Affix does not Override it! \n"
				"Please re-check the asset!"),x, *InAffixTypeName, InIndex));
			InContext.AddError(ErrorMessage);
		}
	}
		
	for (int32 y = 0; y < AffixValuesDefinition.PossibleAffixRanges.Num(); y++)
	{
		const FObsidianAffixValueRange Range = AffixValuesDefinition.PossibleAffixRanges[y];
		if (Range.AffixRanges.Num() != ExpectedCount)
		{
			Result = EDataValidationResult::Invalid;

			const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Number of AffixRanges at index [%i]"
				" inside [%s] Affix at index [%i] differs from expected number of [%d]! \n Please make sure that every"
				" entry has the same number of possible ranges."), y, *InAffixTypeName, InIndex, ExpectedCount));
			InContext.AddError(ErrorMessage);
		}
	}

	return Result;
}
#endif

// ~ FObsidianDynamicItemAffix

FObsidianDynamicItemAffix::operator bool() const
{
	return AffixTag.IsValid();
}

bool FObsidianDynamicItemAffix::operator==(const FObsidianDynamicItemAffix& InOther) const
{
	return AffixTag == InOther.AffixTag;
}

bool FObsidianDynamicItemAffix::operator==(const FObsidianActiveItemAffix& InOther) const
{
	return AffixTag == InOther.AffixTag;
}

bool FObsidianDynamicItemAffix::operator==(const FObsidianStaticItemAffix& InOther) const
{
	return AffixTag == InOther.AffixTag;
}

// ~ FObsidianActiveItemAffix

bool FObsidianActiveItemAffix::operator==(const FObsidianActiveItemAffix& InOther) const
{
	return AffixTag == InOther.AffixTag;
}

bool FObsidianActiveItemAffix::operator==(const FObsidianDynamicItemAffix& InOther) const
{
	return AffixTag == InOther.AffixTag;
}

bool FObsidianActiveItemAffix::operator==(const FObsidianStaticItemAffix& InOther) const
{
	return AffixTag == InOther.AffixTag;
}

uint8 FObsidianActiveItemAffix::GetCurrentAffixTier() const
{
	return CurrentAffixValue.AffixTier.AffixTierValue;
}

int8 FObsidianActiveItemAffix::GetCurrentAffixTierItemLevelRequirement() const
{
	return CurrentAffixValue.AffixTier.MinItemLevelRequirement;
}

void FObsidianActiveItemAffix::InitializeWithDynamic(const FObsidianDynamicItemAffix& InDynamicItemAffix,
	const uint8 InUpToTreasureQuality, const bool bInApplyMagicMultiplier)
{
	if (!InDynamicItemAffix)
	{
		UE_LOG(ObLogAffixes, Warning, TEXT("Initializing Affix failed, InDynamicItemAffix is invalid."))
		return;
	}

	AffixTag = InDynamicItemAffix.AffixTag;
	UnformattedAffixDescription = InDynamicItemAffix.AffixDescription.ToString();
	AffixItemNameAddition = InDynamicItemAffix.AffixItemNameAddition;
	AffixType = InDynamicItemAffix.AffixType;
	AffixValuesDefinition = InDynamicItemAffix.AffixValuesDefinition;
	SoftAbilitySetToApply = InDynamicItemAffix.SoftAbilitySetToApply;

	InitializeAffixTierAndRange(InUpToTreasureQuality, bInApplyMagicMultiplier);
}

void FObsidianActiveItemAffix::InitializeWithStatic(const FObsidianStaticItemAffix& InStaticItemAffix,
	const uint8 InUpToTreasureQuality, const bool bInApplyMagicMultiplier)
{
	if (!InStaticItemAffix)
	{
		UE_LOG(ObLogAffixes, Warning, TEXT("Initializing Affix failed, InStaticItemAffix is invalid."))
		return;
	}

	AffixTag = InStaticItemAffix.AffixTag;
	UnformattedAffixDescription = InStaticItemAffix.AffixDescription.ToString();
	AffixItemNameAddition = InStaticItemAffix.AffixItemNameAddition;
	AffixType = InStaticItemAffix.AffixType;
	AffixValuesDefinition = InStaticItemAffix.AffixValuesDefinition;
	SoftAbilitySetToApply = InStaticItemAffix.SoftAbilitySetToApply;

	InitializeAffixTierAndRange(InUpToTreasureQuality, bInApplyMagicMultiplier);
}

void FObsidianActiveItemAffix::InitializeAffixTierAndRange(const uint8 InUpToTreasureQuality, const bool bInApplyMagicMultiplier)
{
	FObsidianAffixValueRange ChosenAffixValueTier = GetRandomAffixRange(InUpToTreasureQuality);
	const float AffixMultiplier = bInApplyMagicMultiplier ? AffixValuesDefinition.MagicItemAffixRollMultiplier : 1.0f;
	for (int32 i = 0; i < ChosenAffixValueTier.AffixRanges.Num(); ++i)
	{
		FFloatRange& AffixRange = ChosenAffixValueTier.AffixRanges[i];
		float RandomisedValue = FMath::FRandRange(AffixRange.GetLowerBoundValue(), AffixRange.GetUpperBoundValue())
			* AffixMultiplier;
		RandomisedValue = AffixValuesDefinition.AffixValueType == EObsidianAffixValueType::Int ?
				FMath::FloorToInt(RandomisedValue) : FMath::RoundToFloat(RandomisedValue * 100.0f) / 100.0f;
		
		CurrentAffixValue.AffixValuesIdentifiers.Add(AffixValuesDefinition.AffixValuesIdentifiers[i]);	
		CurrentAffixValue.AffixValues.Add(RandomisedValue);	
	}
	CurrentAffixValue.AffixTier = ChosenAffixValueTier.AffixTier;
	CreateAffixActiveDescription();
}

void FObsidianActiveItemAffix::RandomizeAffixValueBoundByRange()
{
	TArray<FFloatRange> CurrentPossibleFloatRanges;
	for (const FObsidianAffixValueRange& AffixValueRange : AffixValuesDefinition.PossibleAffixRanges)
	{
		if (AffixValueRange.AffixTier.AffixTierValue == CurrentAffixValue.AffixTier.AffixTierValue)
		{
			CurrentPossibleFloatRanges = AffixValueRange.AffixRanges;
		}
	}
	
	check(CurrentPossibleFloatRanges.IsEmpty() == false);
	for (int32 i = 0; i < CurrentPossibleFloatRanges.Num(); ++i)
	{
		FFloatRange& AffixRange = CurrentPossibleFloatRanges[i];
		float RandomisedValue = FMath::FRandRange(AffixRange.GetLowerBoundValue(), AffixRange.GetUpperBoundValue());
		RandomisedValue = AffixValuesDefinition.AffixValueType == EObsidianAffixValueType::Int ?
				FMath::FloorToInt(RandomisedValue) : FMath::RoundToFloat(RandomisedValue * 100.0f) / 100.0f;
		
		CurrentAffixValue.AffixValuesIdentifiers.Add(AffixValuesDefinition.AffixValuesIdentifiers[i]);	
		CurrentAffixValue.AffixValues.Add(RandomisedValue);	
	}
	
	CreateAffixActiveDescription();
}

FObsidianAffixValueRange FObsidianActiveItemAffix::GetRandomAffixRange(const uint8 InUpToTreasureQuality)
{
	checkf(!AffixValuesDefinition.PossibleAffixRanges.IsEmpty(), TEXT("Item Affix [%s] has no possible Affix"
		" Ranges filled."), *AffixTag.GetTagName().ToString());
	
	uint32 TotalWeight = 0;
	TArray<FObsidianAffixValueRange> CanRollFromAffixRanges;
	for (const FObsidianAffixValueRange& Value : AffixValuesDefinition.PossibleAffixRanges)
	{
		if (Value.AffixTier.MinItemLevelRequirement <= InUpToTreasureQuality)
		{
			TotalWeight += Value.AffixTierWeight;
			CanRollFromAffixRanges.Add(Value);
		}
	}

	const uint32 Roll = FMath::RandRange(0, TotalWeight);
	uint32 Cumulative = 0;
	
	for (const FObsidianAffixValueRange& Value : CanRollFromAffixRanges)
	{
		Cumulative += Value.AffixTierWeight;
		if (Roll <= Cumulative)
		{
			return Value;
		}
	}

	checkf(false, TEXT("No Affix Range was returned from GetRandomAffixRange."))
	return FObsidianAffixValueRange();
}

void FObsidianActiveItemAffix::CreateAffixActiveDescription()
{
	FStringFormatOrderedArguments Args;
	for (const float AffixValue : CurrentAffixValue.AffixValues)
	{
		if (AffixValuesDefinition.AffixValueType == EObsidianAffixValueType::Int)
		{
			Args.Add(static_cast<int32>(AffixValue));
		}
		else
		{
			Args.Add(AffixValue);
		}
	}
	ActiveAffixDescription = FString::Format(*UnformattedAffixDescription, Args);
}

// ~ FObsidianRareItemNameGenerationData

FText FObsidianRareItemNameGenerationData::GetRandomPrefixNameAddition(const int32 InUpToTreasureQuality)
{
	TArray<FText> PrefixAdditionsCandidates;
	for (const FObsidianRareItemNameAddition& PrefixAdditions : PrefixNameAdditions)
	{
		if (PrefixAdditions.ItemLevelRange.X > InUpToTreasureQuality)
		{
			continue;
		}

		PrefixAdditionsCandidates.Append(PrefixAdditions.ItemNameAdditions);
	}

	if (!ensureMsgf(PrefixAdditionsCandidates.IsEmpty() == false, TEXT("There are no Rare item prefix name additions for item level [%d]."),
		InUpToTreasureQuality))
	{
		return FText::GetEmpty();
	}
	const int32 RandomInt = FMath::RandRange(0, PrefixAdditionsCandidates.Num() - 1);
	return PrefixAdditionsCandidates[RandomInt];
}

FText FObsidianRareItemNameGenerationData::GetRandomSuffixNameAddition(const int32 InUpToTreasureQuality,
	const FGameplayTag& InForItemCategory)
{
	TArray<FText> PrefixAdditionsCandidates;
	for (const FObsidianRareItemSuffixNameAddition& SuffixAdditionClass : SuffixNameAdditions)
	{
		if (SuffixAdditionClass.ForItemCategories.HasTagExact(InForItemCategory) == false)
		{
			continue;
		}
		
		for (const FObsidianRareItemNameAddition& SuffixAdditions : SuffixAdditionClass.ItemNameAdditions)
		{
			if (SuffixAdditions.ItemLevelRange.X > InUpToTreasureQuality)
			{
				continue;
			}

			PrefixAdditionsCandidates.Append(SuffixAdditions.ItemNameAdditions);
		}
	}
	
	if (!ensureMsgf(PrefixAdditionsCandidates.IsEmpty() == false, TEXT("There are no Rare item suffix name additions for [%s] at item level [%d]."),
		*InForItemCategory.ToString(), InUpToTreasureQuality))
	{
		return FText::GetEmpty();
	}
	const int32 RandomInt = FMath::RandRange(0, PrefixAdditionsCandidates.Num() - 1);
	return PrefixAdditionsCandidates[RandomInt];
}

void FObsidianItemRequirementsUIDescription::SetHeroLevelRequirement(const uint8 InRequiredMagnitude,
	const uint8 InOwnerMagnitude)
{
	if (InRequiredMagnitude > 0)
	{
		bHasLevelRequirement = true;
		LevelRequirement = InRequiredMagnitude;
		bMeetLevelRequirement = InOwnerMagnitude >= InRequiredMagnitude;
	}
}

void FObsidianItemRequirementsUIDescription::SetHeroClassRequirement(const EObsidianHeroClass InRequiredClass,
	const EObsidianHeroClass InOwnerClass)
{
	if (InRequiredClass > EObsidianHeroClass::None)
	{
		bHasHeroClassRequirement = true;
		HeroClassRequirementText = UObsidianGameplayStatics::GetHeroClassText(InRequiredClass);
		bMeetHeroClassRequirement = InOwnerClass == InRequiredClass;
	}
}

void FObsidianItemRequirementsUIDescription::SetAttributeRequirement(const FGameplayAttribute& InAttribute,
	const float InRequirementMagnitude, const float InOwnerMagnitude)
{
	if (InRequirementMagnitude <= 0)
	{
		return;
	}
	
	ensureMsgf(InAttribute.GetAttributeSetClass() == UObsidianHeroAttributeSet::StaticClass(),
		TEXT("Attribute [%s] belongs to [%s], assumed ObsidianHeroAttributeSet, "
	    "please update FObsidianItemRequirementsUIDescription::SetAttributeRequirement logic."),
		*InAttribute.GetName(), *GetNameSafe(InAttribute.GetAttributeSetClass()));
	
	if (InAttribute == UObsidianHeroAttributeSet::GetStrengthAttribute())
	{
		bHasStrengthRequirement = true;
		StrengthRequirement = InRequirementMagnitude;
		bMeetStrengthRequirement = InOwnerMagnitude >= InRequirementMagnitude;
	}
	else if (InAttribute == UObsidianHeroAttributeSet::GetDexterityAttribute())
	{
		bHasDexterityRequirement = true;
		DexterityRequirement = InRequirementMagnitude;
		bMeetDexterityRequirement = InOwnerMagnitude >= InRequirementMagnitude;
	}
	else if (InAttribute == UObsidianHeroAttributeSet::GetFaithAttribute())
	{
		bHasFaithRequirement = true;
		FaithRequirement = InRequirementMagnitude;
		bMeetFaithRequirement = InOwnerMagnitude >= InRequirementMagnitude;
	}
	else if (InAttribute == UObsidianHeroAttributeSet::GetIntelligenceAttribute())
	{
		bHasIntelligenceRequirement = true;
		IntelligenceRequirement = InRequirementMagnitude;
		bMeetIntelligenceRequirement = InOwnerMagnitude >= InRequirementMagnitude;
	}
}

// ~ FObsidianDescriptionAffixRow

void FObsidianAffixDescriptionRow::SetAffixAdditionalDescription(const EObsidianAffixType& InAffixType,
	const int32 InAffixTier)
{
	AffixType = InAffixType;
	FText AffixTypeText = FText();
	
	switch (InAffixType)
	{
	case EObsidianAffixType::Implicit:
		AffixTypeText = FText::FromString(FString::Printf(TEXT("Implicit, ")));
		break;
	case EObsidianAffixType::Prefix:
		AffixTypeText = FText::FromString(FString::Printf(TEXT("Prefix, ")));
		break;
	case EObsidianAffixType::Suffix:
		AffixTypeText = FText::FromString(FString::Printf(TEXT("Suffix, ")));
		break;
	default:
		AffixTypeText = FText::FromString(FString::Printf(TEXT("Affix, ")));
		break;
	}
	
	AffixAdditionalDescription = FText::FromString(FString::Printf(TEXT("%s tier: %d "), *AffixTypeText.ToString(),
		InAffixTier));
}

// ~ FObsidianItemStats

void FObsidianItemStats::SetItemImage(UTexture2D* InItemImage, const FIntPoint& InItemGridSpan)
{
	bContainsItemImage = true;
	ItemImage = InItemImage;
	ItemGridSpan = InItemGridSpan;
}

void FObsidianItemStats::SetDisplayName(const FText& InDisplayName)
{
	bContainsDisplayName = true;
	DisplayName = InDisplayName;
}

void FObsidianItemStats::SetItemLevel(const int8 InItemLevel)
{
	bContainsItemLevel = true;
	ItemLevel = InItemLevel;
}

void FObsidianItemStats::SetRareDisplayNameAddition(const FString& InDisplayNameAddition)
{
	if (InDisplayNameAddition.IsEmpty() == false) // This will be empty for anything other than Rare or Magic item.
	{
		bContainsRareItemDisplayNameAddition = true;
		RareItemDisplayNameAddition = InDisplayNameAddition;
	}
}

void FObsidianItemStats::SetMagicDisplayNameAddition(const FString& InDisplayNameAddition)
{
	if (InDisplayNameAddition.IsEmpty() == false) // This will be empty for anything other than Rare or Magic item.
	{
		bContainsMagicItemDisplayNameAddition = true;
		MagicItemDisplayNameAddition = InDisplayNameAddition;
	}
}

void FObsidianItemStats::SetDescription(const FText& InDescription)
{
	bContainsDescription = true;
	Description = InDescription;
}

void FObsidianItemStats::SetAdditionalDescription(const FText& InAdditionalDescription)
{
	bContainsAdditionalDescription = true;
	AdditionalDescription = InAdditionalDescription;
}

void FObsidianItemStats::SetStacks(const int32 InCurrentStack, const int32 InMaxStacks)
{
	bContainsStacks = true;
	StacksData = FObsidianStacksUIData(InCurrentStack, InMaxStacks);
}

void FObsidianItemStats::SetCurrentStacks(const int32 InCurrentStack)
{
	bContainsStacks = true;
	StacksData.SetCurrentStacks(InCurrentStack);
}

void FObsidianItemStats::SetMaxStacks(const int32 InMaxStacks)
{
	bContainsStacks = true;
	StacksData.SetMaxStacks(InMaxStacks);
}

void FObsidianItemStats::SetIdentified(const bool InIdentified)
{
	bSupportIdentification = true;
	bIdentified = InIdentified;
}

void FObsidianItemStats::SetAffixDescriptionRows(const TArray<FObsidianAffixDescriptionRow>& InAffixRows)
{
	bContainsAffixes = true;
	AffixDescriptionRows = InAffixRows;
}

void FObsidianItemStats::SetItemEquippingRequirements(const FObsidianItemRequirementsUIDescription& InRequirements)
{
	bHasItemEquippingRequirements = true;
	ItemEquippingRequirements = InRequirements;
}

// ~ End of FObsidianDescriptionAffixRow



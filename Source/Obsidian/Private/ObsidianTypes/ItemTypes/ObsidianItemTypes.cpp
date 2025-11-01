// Copyright 2024 out of sCope team - Michał Ogiński

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"


#include "InventoryItems/ItemAffixes/ObsidianItemAffixStack.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "Core/ObsidianGameplayStatics.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "Obsidian/ObsidianGameModule.h"

// ~ FObsidianItemGeneratedData

void FObsidianItemGeneratedData::Reset()
{
	AvailableStackCount = 1;
	ItemRarity = EObsidianItemRarity::None;
	ItemAffixes.Reset();
	NameData = FObsidianItemGeneratedNameData();
	ItemEquippingRequirements = FObsidianItemRequirements();
}

// ~ FDraggedItem

FDraggedItem::FDraggedItem(UObsidianInventoryItemInstance* InInstance) 
	: Instance(InInstance)
	, GeneratedData(InInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current)) //TODO(intrxx) do I really need stacks here?
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

EObsidianPlacingAtSlotResult FObsidianSlotDefinition::CanPlaceAtSlot(const FGameplayTag& ItemCategory) const
{
	if(BannedItemCategories.HasTagExact(ItemCategory))
	{
		return EObsidianPlacingAtSlotResult::UnableToPlace_BannedCategory;
	}
	
	if(AcceptedItemCategories.HasTagExact(ItemCategory))
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

void FObsidianSlotDefinition::RemoveBannedItemCategory(const FGameplayTag& BannedCategoryToRemove)
{
#if !UE_BUILD_SHIPPING
	if(BannedItemCategories.HasTag(BannedCategoryToRemove) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Trying to remove Banned Equipment Tag [%s] but the Tag does not exist in BannedItemCategories."), *BannedCategoryToRemove.ToString());
	}
#endif
	BannedItemCategories.RemoveTag(BannedCategoryToRemove);
}

void FObsidianSlotDefinition::RemoveBannedItemCategories(const FGameplayTagContainer& BannedCategoriesToRemove)
{
#if !UE_BUILD_SHIPPING
	for(FGameplayTag Tag : BannedItemCategories)
	{
		if(BannedItemCategories.HasTag(Tag) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("Trying to remove Banned Equipment Tag [%s] but the Tag does not exist in BannedItemCategories."), *Tag.ToString());
		}
	}
#endif
	BannedItemCategories.RemoveTags(BannedCategoriesToRemove);
}

// ~ FObsidianItemPosition

bool FObsidianItemPosition::IsPositionedOnGrid() const
{
	return (GridLocation != FIntPoint::NoneValue) && (SlotTag == FGameplayTag::EmptyTag);
}

bool FObsidianItemPosition::IsPositionedAtSlot() const
{
	return (GridLocation == FIntPoint::NoneValue) && (SlotTag != FGameplayTag::EmptyTag);
}

FIntPoint FObsidianItemPosition::GetItemGridLocation(const bool bWarnIfNotFound) const
{
#if !UE_BUILD_SHIPPING
	if(bWarnIfNotFound && GridLocation == FIntPoint::NoneValue)
	{
		UE_LOG(LogObsidian, Error, TEXT("Grid Location is invalid in [%hs]."), ANSI_TO_TCHAR(__FUNCTION__));
	}
#endif
	return GridLocation;
}

FGameplayTag FObsidianItemPosition::GetItemSlotTag(const bool bWarnIfNotFound) const
{
#if !UE_BUILD_SHIPPING
	if(bWarnIfNotFound && SlotTag == FGameplayTag::EmptyTag)
	{
		UE_LOG(LogObsidian, Error, TEXT("Slot Tag is invalid in [%hs]."), ANSI_TO_TCHAR(__FUNCTION__));
	}
#endif
	return SlotTag;
}

FGameplayTag FObsidianItemPosition::GetOwningStashTabTag() const
{
	return OwningStashTabTag;
}

void FObsidianItemPosition::SetOwningStashTab(const FGameplayTag& InOwningStashTab)
{
	OwningStashTabTag = InOwningStashTab;
}

FString FObsidianItemPosition::GetDebugStringPosition() const
{
	if (OwningStashTabTag != FGameplayTag::EmptyTag)
	{
		if (SlotTag != FGameplayTag::EmptyTag)
		{
			return FString::Printf(TEXT("Stash Tab: [%s], Slot: [%s]"), *OwningStashTabTag.GetTagName().ToString(), *SlotTag.GetTagName().ToString());
		}
		if (GridLocation != FIntPoint::NoneValue)
		{
			return FString::Printf(TEXT("Stash Tab: [%s], Grid Location: [%d, %d]"), *OwningStashTabTag.GetTagName().ToString(), GridLocation.X, GridLocation.Y);
		}
	}
	else
	{
		if (SlotTag != FGameplayTag::EmptyTag)
		{
			return FString::Printf(TEXT("Equipment Slot: [%s]"), *SlotTag.GetTagName().ToString());
		}
		if (GridLocation != FIntPoint::NoneValue)
		{
			return FString::Printf(TEXT("Inventory Grid Location: [%d, %d]"), GridLocation.X, GridLocation.Y);
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

bool FObsidianStaticItemAffix::operator==(const FObsidianStaticItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

bool FObsidianStaticItemAffix::operator==(const FObsidianDynamicItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

bool FObsidianStaticItemAffix::operator==(const FObsidianActiveItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

// ~ FObsidianDynamicItemAffix

FObsidianDynamicItemAffix::operator bool() const
{
	return AffixTag.IsValid();
}

bool FObsidianDynamicItemAffix::operator==(const FObsidianDynamicItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

bool FObsidianDynamicItemAffix::operator==(const FObsidianActiveItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

bool FObsidianDynamicItemAffix::operator==(const FObsidianStaticItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

// ~ FObsidianActiveItemAffix

bool FObsidianActiveItemAffix::operator==(const FObsidianActiveItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

bool FObsidianActiveItemAffix::operator==(const FObsidianDynamicItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

bool FObsidianActiveItemAffix::operator==(const FObsidianStaticItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

uint8 FObsidianActiveItemAffix::GetCurrentAffixTier() const
{
	return CurrentAffixValue.AffixTier.AffixTierValue;
}

uint8 FObsidianActiveItemAffix::GetCurrentAffixTierItemLevelRequirement() const
{
	return CurrentAffixValue.AffixTier.MinItemLevelRequirement;
}

void FObsidianActiveItemAffix::InitializeWithDynamic(const FObsidianDynamicItemAffix& InDynamicItemAffix, const uint8 UpToTreasureQuality,
                                                     const bool bApplyMultiplier)
{
	if (!InDynamicItemAffix)
	{
		UE_LOG(LogAffixes, Warning, TEXT("Initializing Affix failed, InDynamicItemAffix is invalid."))
		return;
	}

	AffixTag = InDynamicItemAffix.AffixTag;
	UnformattedAffixDescription = InDynamicItemAffix.AffixDescription;
	AffixItemNameAddition = InDynamicItemAffix.AffixItemNameAddition;
	AffixType = InDynamicItemAffix.AffixType;
	AffixValuesDefinition = InDynamicItemAffix.AffixValuesDefinition;
	SoftAbilitySetToApply = InDynamicItemAffix.SoftAbilitySetToApply;

	InitializeAffixTierAndRange(UpToTreasureQuality, bApplyMultiplier);
}

void FObsidianActiveItemAffix::InitializeWithStatic(const FObsidianStaticItemAffix& InStaticItemAffix, const uint8 UpToTreasureQuality,
	const bool bApplyMultiplier)
{
	if (!InStaticItemAffix)
	{
		UE_LOG(LogAffixes, Warning, TEXT("Initializing Affix failed, InStaticItemAffix is invalid."))
		return;
	}

	AffixTag = InStaticItemAffix.AffixTag;
	UnformattedAffixDescription = InStaticItemAffix.AffixDescription;
	AffixItemNameAddition = InStaticItemAffix.AffixItemNameAddition;
	AffixType = InStaticItemAffix.AffixType;
	AffixValuesDefinition = InStaticItemAffix.AffixValuesDefinition;
	SoftAbilitySetToApply = InStaticItemAffix.SoftAbilitySetToApply;

	InitializeAffixTierAndRange(UpToTreasureQuality, bApplyMultiplier);
}

void FObsidianActiveItemAffix::InitializeAffixTierAndRange(const uint8 UpToTreasureQuality, const bool bApplyMultiplier)
{
	FObsidianAffixValueRange ChosenAffixValueTier = GetRandomAffixRange(UpToTreasureQuality);
	const float AffixMultiplier = bApplyMultiplier ? AffixValuesDefinition.MagicItemAffixRollMultiplier : 1.0f;
	for (int32 i = 0; i < ChosenAffixValueTier.AffixRanges.Num(); ++i)
	{
		FFloatRange& AffixRange = ChosenAffixValueTier.AffixRanges[i];
		float RandomisedValue = FMath::FRandRange(AffixRange.GetLowerBoundValue(), AffixRange.GetUpperBoundValue()) * AffixMultiplier;
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

FObsidianAffixValueRange FObsidianActiveItemAffix::GetRandomAffixRange(const uint8 UpToTreasureQuality)
{
	uint32 TotalWeight = 0;
	TArray<FObsidianAffixValueRange> CanRollFromAffixRanges;
	for (const FObsidianAffixValueRange& Value : AffixValuesDefinition.PossibleAffixRanges)
	{
		if (Value.AffixTier.MinItemLevelRequirement <= UpToTreasureQuality)
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
	TArray<FFormatArgumentValue> Args;
	for (const float AffixValue : CurrentAffixValue.AffixValues)
	{
		Args.Add(AffixValue);
	}
	ActiveAffixDescription = FText::Format(UnformattedAffixDescription, Args);
}

// ~ FObsidianRareItemNameGenerationData

FText FObsidianRareItemNameGenerationData::GetRandomPrefixNameAddition(const int32 UpToTreasureQuality)
{
	TArray<FText> PrefixAdditionsCandidates;
	for (const FObsidianRareItemNameAddition& PrefixAdditions : PrefixNameAdditions)
	{
		if (PrefixAdditions.ItemLevelRange.X > UpToTreasureQuality)
		{
			continue;
		}

		PrefixAdditionsCandidates.Append(PrefixAdditions.ItemNameAdditions);
	}

	check(PrefixAdditionsCandidates.IsEmpty() == false);
	const int32 RandomInt = FMath::RandRange(0, PrefixAdditionsCandidates.Num() - 1);
	return PrefixAdditionsCandidates[RandomInt];
}

FText FObsidianRareItemNameGenerationData::GetRandomSuffixNameAddition(const int32 UpToTreasureQuality, const FGameplayTag& ForItemCategory)
{
	TArray<FText> PrefixAdditionsCandidates;
	for (const FObsidianRareItemSuffixNameAddition& SuffixAdditionClass : SuffixNameAdditions)
	{
		if (SuffixAdditionClass.ForItemCategories.HasTagExact(ForItemCategory) == false)
		{
			continue;
		}
		
		for (const FObsidianRareItemNameAddition& SuffixAdditions : SuffixAdditionClass.ItemNameAdditions)
		{
			if (SuffixAdditions.ItemLevelRange.X > UpToTreasureQuality)
			{
				continue;
			}

			PrefixAdditionsCandidates.Append(SuffixAdditions.ItemNameAdditions);
		}
	}
	
	check(PrefixAdditionsCandidates.IsEmpty() == false);
	const int32 RandomInt = FMath::RandRange(0, PrefixAdditionsCandidates.Num() - 1);
	return PrefixAdditionsCandidates[RandomInt];
}

void FObsidianItemRequirementsUIDescription::SetHeroLevelRequirement(const uint8 RequiredMagnitude, const uint8 OwnerMagnitude)
{
	if (RequiredMagnitude > 0)
	{
		bHasLevelRequirement = true;
		LevelRequirement = RequiredMagnitude;
		bMeetLevelRequirement = OwnerMagnitude >= RequiredMagnitude;
	}
}

void FObsidianItemRequirementsUIDescription::SetHeroClassRequirement(const EObsidianHeroClass RequiredClass, const EObsidianHeroClass OwnerClass)
{
	if (RequiredClass > EObsidianHeroClass::None)
	{
		bHasHeroClassRequirement = true;
		HeroClassRequirementText = UObsidianGameplayStatics::GetHeroClassText(RequiredClass);
		bMeetHeroClassRequirement = OwnerClass == RequiredClass;
	}
}

void FObsidianItemRequirementsUIDescription::SetAttributeRequirement(const FGameplayAttribute& Attribute, const float RequirementMagnitude,
                                                                     const float OwnerMagnitude)
{
	if (RequirementMagnitude <= 0)
	{
		return;
	}
	
	ensureMsgf(Attribute.GetAttributeSetClass() == UObsidianHeroAttributeSet::StaticClass(),
		TEXT("Attribute [%s] belongs to [%s], assumed ObsidianHeroAttributeSet, "
	    "please update FObsidianItemRequirementsUIDescription::SetAttributeRequirement logic."),
		*Attribute.GetName(), *GetNameSafe(Attribute.GetAttributeSetClass()));
	
	if (Attribute == UObsidianHeroAttributeSet::GetStrengthAttribute())
	{
		bHasStrengthRequirement = true;
		StrengthRequirement = RequirementMagnitude;
		bMeetStrengthRequirement = OwnerMagnitude >= RequirementMagnitude;
	}
	else if (Attribute == UObsidianHeroAttributeSet::GetDexterityAttribute())
	{
		bHasDexterityRequirement = true;
		DexterityRequirement = RequirementMagnitude;
		bMeetDexterityRequirement = OwnerMagnitude >= RequirementMagnitude;
	}
	else if (Attribute == UObsidianHeroAttributeSet::GetFaithAttribute())
	{
		bHasFaithRequirement = true;
		FaithRequirement = RequirementMagnitude;
		bMeetFaithRequirement = OwnerMagnitude >= RequirementMagnitude;
	}
	else if (Attribute == UObsidianHeroAttributeSet::GetIntelligenceAttribute())
	{
		bHasIntelligenceRequirement = true;
		IntelligenceRequirement = RequirementMagnitude;
		bMeetIntelligenceRequirement = OwnerMagnitude >= RequirementMagnitude;
	}
}

// ~ FObsidianDescriptionAffixRow

void FObsidianAffixDescriptionRow::SetAffixAdditionalDescription(const EObsidianAffixType& InAffixType, const int32 InAffixTier)
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
	
	AffixAdditionalDescription = FText::FromString(FString::Printf(TEXT("%s tier: %d "), *AffixTypeText.ToString(), InAffixTier));
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

void FObsidianItemStats::SetAffixDescriptionRows(const TArray<FObsidianAffixDescriptionRow>& AffixRows)
{
	bContainsAffixes = true;
	AffixDescriptionRows = AffixRows;
}

void FObsidianItemStats::SetItemEquippingRequirements(const FObsidianItemRequirementsUIDescription& Requirements)
{
	bHasItemEquippingRequirements = true;
	ItemEquippingRequirements = Requirements;
}

// ~ End of FObsidianDescriptionAffixRow



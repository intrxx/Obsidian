// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ItemDrop/ObsidianItemDropComponent.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"

#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Equippable.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataLoaderSubsystem.h"
#include "InventoryItems/ItemDrop/ObsidianItemDropManagerSubsystem.h"
#include "InventoryItems/ItemDrop/ObsidianTreasureList.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "Obsidian/ObsidianLogCategories.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif


namespace DropComponentDebugHelpers
{
	const inline TMap<EObsidianItemRarity, FString> ItemRarityToDebugStringMap =
	{
		{EObsidianItemRarity::None, TEXT("None")},
		{EObsidianItemRarity::Quest, TEXT("Quest")},
		{EObsidianItemRarity::Normal, TEXT("Normal")},
		{EObsidianItemRarity::Magic, TEXT("Magic")},
		{EObsidianItemRarity::Rare, TEXT("Rare")},
		{EObsidianItemRarity::Unique, TEXT("Unique")},
		{EObsidianItemRarity::Set, TEXT("Set")},
	};

	inline FString GetRarityDebugString(const EObsidianItemRarity InRarity)
	{
		return ItemRarityToDebugStringMap[InRarity];
	}
}

// ~ FObsidianAdditionalTreasureList

#if WITH_EDITOR
EDataValidationResult FObsidianAdditionalTreasureList::ValidateData(FDataValidationContext& InContext, const int InIndex) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;
	
	if(TreasureList.IsNull())
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Treasure List at index [%i] is empty! \n"
			"Please fill Treasure List or delete this index entry in Additional Treasure Lists"), InIndex));

		InContext.AddError(ErrorMessage);
	}
	
	return Result;
}
#endif

// ~ End of FObsidianAdditionalTreasureList

UObsidianItemDropComponent::UObsidianItemDropComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UObsidianItemDropComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadAdditionalTreasuresAsync();
}

void UObsidianItemDropComponent::LoadAdditionalTreasuresAsync()
{
	TArray<FSoftObjectPath> AdditionalTreasureListsPaths;
	for (const FObsidianAdditionalTreasureList& AdditionalTL : AdditionalTreasureLists)
	{
		AdditionalTreasureListsPaths.Add(AdditionalTL.TreasureList.ToSoftObjectPath());
	}
	
	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(AdditionalTreasureListsPaths);
}

void UObsidianItemDropComponent::DropItems(const EObsidianEntityRarity InDroppingEntityRarity, const uint8 InDroppingEntityLevel,
	const FVector& InOverrideDropLocation)
{
	checkf(InDroppingEntityRarity != EObsidianEntityRarity::None, TEXT("Entity Rarity passed to DropItems is None, setup or run time logic is invalid."));
	
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>();
	if (ItemDataSettings == nullptr)
	{
		UE_LOG(ObLogItemData, Error, TEXT("ItemDataSettings was not found in [%hs]"), __FUNCTION__);
		return;
	}

	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(World);
	if (GameInstance == nullptr)
	{
		UE_LOG(ObLogItemDrop, Error, TEXT("GameInstance is nullptr in [%hs]"), __FUNCTION__);
		return;
	}

	CachedItemDataLoader = CachedItemDataLoader == nullptr ? GameInstance->GetSubsystem<UObsidianItemDataLoaderSubsystem>() : CachedItemDataLoader;
	if (CachedItemDataLoader == nullptr)
	{
		UE_LOG(ObLogItemDrop, Error, TEXT("CachedItemDataLoader is nullptr in [%hs]"), __FUNCTION__);
		return;
	}
	
	const uint8 TreasureQuality = FMath::Clamp(
		(InDroppingEntityLevel + ItemDataSettings->GetDefaultAddedTreasureQualityForEntityRarity(InDroppingEntityRarity)),
		1, ItemDataSettings->MaxTreasureQuality);
	
	TArray<FObsidianTreasureClass> TreasureClasses;
	TArray<FObsidianTreasureClass> MustRollFromTreasureClasses;
	GetTreasureClassesToRollFrom(TreasureQuality, TreasureClasses, MustRollFromTreasureClasses);
	
	if (TreasureClasses.IsEmpty() && MustRollFromTreasureClasses.IsEmpty())
	{
		UE_LOG(ObLogItemDrop, Warning, TEXT("TreasureClasses are empty after getting them from both the common"
									   " set and additional lists, is enemy level to low for drops or something is broken?"));
		return;
	}
	
	uint8 DropRolls = ItemDataSettings->GetDefaultDropRollNumberForEntityRarity(InDroppingEntityRarity);

	for (const FObsidianTreasureClass& TC : TreasureClasses)
	{
		UE_LOG(ObLogItemDrop, Verbose, TEXT("Rolling Items from: [%s]"), *TC.DebugName);
	}
	for (const FObsidianTreasureClass& TC : MustRollFromTreasureClasses)
	{
		UE_LOG(ObLogItemDrop, Verbose, TEXT("Rolling Items from: [%s]"), *TC.DebugName);
	}
	
	TArray<FObsidianItemToDrop> ItemsToDrop;
	if (MustRollFromTreasureClasses.IsEmpty() == false && DropRolls > 0)
	{
		//TODO(intrxx) Get Random TC in some weighted way?
		const uint16 RandomClassIndex = FMath::RandRange(0, (MustRollFromTreasureClasses.Num() - 1));
		const FObsidianDropItem DropItem = MustRollFromTreasureClasses[RandomClassIndex].GetRandomItemFromClass();
		
		FObsidianItemToDrop ItemToDrop;
		if (ConstructItemToDrop(DropItem, InOverrideDropLocation, TreasureQuality, ItemToDrop))
		{
			ItemsToDrop.Add(ItemToDrop);
		}
		DropRolls--;
	}
	
	for (uint8 i = 0; i < DropRolls; ++i)
	{
		//TODO(intrxx) Get Random TC in some weighted way?
		const uint16 RandomClassIndex = FMath::RandRange(0, (TreasureClasses.Num() - 1));
		const FObsidianDropItem DropItem = TreasureClasses[RandomClassIndex].GetRandomItemFromClass();
		
		FObsidianItemToDrop ItemToDrop;
		if (ConstructItemToDrop(DropItem, InOverrideDropLocation, TreasureQuality, ItemToDrop))
		{
			ItemsToDrop.Add(ItemToDrop);
		}
	}

	if (ItemsToDrop.IsEmpty())
	{
		UE_LOG(ObLogItemDrop, Verbose, TEXT("No Items Were Rolled To Drop."));
		OnDroppingItemsFinishedDelegate.Broadcast(false);
		return;
	}
	
	if (const UObsidianItemDropManagerSubsystem* ManagerSubsystem = World->GetSubsystem<UObsidianItemDropManagerSubsystem>())
	{
		ManagerSubsystem->RequestDroppingItems(MoveTemp(ItemsToDrop));
		OnDroppingItemsFinishedDelegate.Broadcast(true);
	}
}

bool UObsidianItemDropComponent::ConstructItemToDrop(const FObsidianDropItem& InDropItem, const FVector& InOverrideDropLocation,
	const uint8 InTreasureQuality, FObsidianItemToDrop& OutItemToDrop)
{
	if (InDropItem.IsValid() == false)
	{
		return false;
	}
	
	const AActor* OwningActor = GetOwner();
	if (OwningActor == nullptr)
	{
		UE_LOG(ObLogItemDrop, Error, TEXT("OwningActor of ItemDropComponent is null in [%hs]"),
			__FUNCTION__);
		return false;
	}

	TSoftClassPtr<UObsidianInventoryItemDefinition> ItemSoftItemDefinition = InDropItem.SoftTreasureItemDefinitionClass;
	const EObsidianItemRarity RolledRarity = InDropItem.bShouldRandomizeRarity ? RollItemRarity(InDropItem.ItemMaxRarity) : EObsidianItemRarity::None;
	if (RolledRarity >= EObsidianItemRarity::Unique)
	{
		FGameplayTag ItemBaseTypeTag = InDropItem.ItemBaseType;
		if (InDropItem.ItemBaseType.IsValid() == false)
		{
			ItemBaseTypeTag = GetItemBaseTypeFromDropItem(InDropItem);
		}
		
		if (CachedItemDataLoader && ItemBaseTypeTag.IsValid())
		{
			FObsidianTreasureClass SpecialItemsTreasureClass;
			CachedItemDataLoader->GetAllUniqueOrSetItemsOfBaseItemTypeUpToQuality(InTreasureQuality, RolledRarity,
				ItemBaseTypeTag, SpecialItemsTreasureClass);
			
			const FObsidianDropItem RolledSpecialItem = SpecialItemsTreasureClass.GetRandomItemFromClass();
			ItemSoftItemDefinition = RolledSpecialItem.SoftTreasureItemDefinitionClass;
		}
	}
	
	OutItemToDrop.ItemDefinitionClass = ItemSoftItemDefinition.Get();
	if (OutItemToDrop.ItemDefinitionClass == nullptr)
	{
		UE_LOG(ObLogItemDrop, Warning, TEXT("ItemToDrop was not loaded previously, falling back to LoadSynchronous in [%hs]."),
			__FUNCTION__);
		OutItemToDrop.ItemDefinitionClass = ItemSoftItemDefinition.LoadSynchronous();
	}

	OutItemToDrop.DropItemLevel = InTreasureQuality;
	OutItemToDrop.DropRarity = InDropItem.bShouldRandomizeRarity ? RolledRarity : GetItemDefaultRarityFromDropItem(InDropItem);
	OutItemToDrop.bShouldApplyMultiplier = ShouldApplyAffixValueMultiplier(OutItemToDrop.DropRarity);
	OutItemToDrop.DropTransform = GetDropTransformAligned(OwningActor, InOverrideDropLocation);
	OutItemToDrop.DropStacks = InDropItem.GetRandomStackSizeToDropAdjusted(InTreasureQuality);
	ConstructItem(OutItemToDrop);
	return true;
}

void UObsidianItemDropComponent::ConstructItem(FObsidianItemToDrop& InOutForItemToDrop)
{
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = InOutForItemToDrop.ItemDefinitionClass;
	if (ItemDef == nullptr)
	{
		UE_LOG(ObLogItemDrop, Error, TEXT("ItemDef is invalid in [%hs]."), __FUNCTION__);
		return;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if (DefaultObject == nullptr)
	{
		UE_LOG(ObLogItemDrop, Error, TEXT("DefaultObject of SoftTreasureItemDefinitionClass is invalid,"
									   " abandoning [%hs]"), __FUNCTION__);
		return;
	}

	const UOInventoryItemFragment_Affixes* AffixFragment = Cast<UOInventoryItemFragment_Affixes>(
		DefaultObject->FindFragmentByClass(UOInventoryItemFragment_Affixes::StaticClass()));
	if (AffixFragment == nullptr)
	{
		//NOTE(intrxx) there is no Affix Fragment so there is nothing more to generate.
		return;		
	}

	if (InOutForItemToDrop.DropRarity == EObsidianItemRarity::Rare)
	{
		InOutForItemToDrop.DropRareItemDisplayNameAddition = CachedItemDataLoader->GetRandomRareItemNameAddition(
			InOutForItemToDrop.DropItemLevel, DefaultObject->GetItemCategoryTag());
	}
	else if (InOutForItemToDrop.bShouldApplyMultiplier) // Only magic items can apply affix multiplier, that's why it's else if.
	{
		InOutForItemToDrop.DropMagicItemDisplayNameAddition = CachedItemDataLoader->GetAffixMultiplierMagicItemNameAddition();
	}
	
	switch (AffixFragment->GetGenerationType())
	{
		case EObsidianAffixGenerationType::DefaultGeneration:
			{
				HandleDefaultGeneration(InOutForItemToDrop, DefaultObject->GetItemCategoryTag(), DefaultObject->GetItemBaseTypeTag(),
					AffixFragment);
			} break;
		case EObsidianAffixGenerationType::FullGeneration:
			{
				HandleFullGeneration(InOutForItemToDrop, DefaultObject->GetItemCategoryTag(), DefaultObject->GetItemBaseTypeTag(),
					AffixFragment);
			} break;
		case EObsidianAffixGenerationType::NoGeneration:
			{
				HandleNoGeneration(InOutForItemToDrop, AffixFragment);
			} break;
			default:
			{} break;
	}

	const UOInventoryItemFragment_Equippable* EquippableFragment = Cast<UOInventoryItemFragment_Equippable>(
		DefaultObject->FindFragmentByClass(UOInventoryItemFragment_Equippable::StaticClass()));
	if (EquippableFragment == nullptr)
	{
		return;		
	}

	FObsidianItemRequirements DefaultRequirements = EquippableFragment->GetItemDefaultEquippingRequirements();
	if (UObsidianItemsFunctionLibrary::HasEquippingRequirements(DefaultRequirements))
	{
		AdjustItemRequirementsBasedOnAddedAffixes(DefaultRequirements, InOutForItemToDrop);
		InOutForItemToDrop.DropItemRequirements = DefaultRequirements;
	}
}

void UObsidianItemDropComponent::HandleDefaultGeneration(FObsidianItemToDrop& InOutForItemToDrop, const FGameplayTag& InDropItemCategory,
	const FGameplayTag& InDropItemBaseTypeTag, const UOInventoryItemFragment_Affixes* InAffixFragment)
{
	TryToGivePrimaryItemAffix(InOutForItemToDrop, InAffixFragment);
	TryToGiveStaticImplicit(InOutForItemToDrop, InAffixFragment);
	
	if (InOutForItemToDrop.DropRarity != EObsidianItemRarity::Normal)
	{
		TArray<FObsidianDynamicItemAffix> PrefixAffixes;
		TArray<FObsidianDynamicItemAffix> SuffixAffixes;
		TArray<FObsidianDynamicItemAffix> SkillImplicitAffixes;
		const bool bGatheredAffixes = CachedItemDataLoader->GetAllAffixesUpToQualityForCategory_DefaultGeneration(
			InOutForItemToDrop.DropItemLevel, InDropItemCategory, InDropItemBaseTypeTag,
			/** OUT */ PrefixAffixes,
			/** OUT */ SuffixAffixes,
			/** OUT */ SkillImplicitAffixes);
		if (bGatheredAffixes == false)
		{
			UE_LOG(ObLogItemDrop, Warning, TEXT("Could not find any Affixes for [%s] up to [%d] quality level."),
				*InDropItemCategory.GetTagName().ToString(), InOutForItemToDrop.DropItemLevel);
			return;
		}

		RollSkillImplicits(InOutForItemToDrop, SkillImplicitAffixes);
		RollAffixesAndPrefixes(InOutForItemToDrop, PrefixAffixes, SuffixAffixes);
	}
	else if (InOutForItemToDrop.DropRarity == EObsidianItemRarity::Normal)
	{
		TArray<FObsidianDynamicItemAffix> SkillImplicitAffixes;
		const bool bGatheredAffixes = CachedItemDataLoader->GetAllSkillImplicitsUpToQualityForCategory(
			InOutForItemToDrop.DropItemLevel, InDropItemCategory, InDropItemBaseTypeTag,
			/** OUT */ SkillImplicitAffixes);
		if (bGatheredAffixes == false)
		{
			UE_LOG(ObLogItemDrop, Warning, TEXT("Could not find any Skill Implicits for [%s] up to [%d] quality level."),
				*InDropItemCategory.GetTagName().ToString(), InOutForItemToDrop.DropItemLevel);
			return;
		}

		RollSkillImplicits(InOutForItemToDrop, SkillImplicitAffixes);
	}
}

void UObsidianItemDropComponent::HandleFullGeneration(FObsidianItemToDrop& InOutForItemToDrop, const FGameplayTag& InDropItemCategory,
	const FGameplayTag& InDropItemBaseTypeTag, const UOInventoryItemFragment_Affixes* InAffixFragment)
{
	TryToGivePrimaryItemAffix(InOutForItemToDrop, InAffixFragment);
	
	if (InOutForItemToDrop.DropRarity != EObsidianItemRarity::Normal)
	{
		TArray<FObsidianDynamicItemAffix> ImplicitAffixes;
		TArray<FObsidianDynamicItemAffix> PrefixAffixes;
		TArray<FObsidianDynamicItemAffix> SuffixAffixes;
		TArray<FObsidianDynamicItemAffix> SkillImplicitAffixes;
		const bool bGatheredAffixes = CachedItemDataLoader->GetAllAffixesUpToQualityForCategory_FullGeneration(
			InOutForItemToDrop.DropItemLevel, InDropItemCategory, InDropItemBaseTypeTag,
			/** OUT */ PrefixAffixes,
			/** OUT */ SuffixAffixes,
			/** OUT */ ImplicitAffixes,
			/** OUT */ SkillImplicitAffixes);
		if (bGatheredAffixes == false)
		{
			UE_LOG(ObLogItemDrop, Warning, TEXT("Could not find any Affixes for [%s] up to [%d] quality level."),
				*InDropItemCategory.GetTagName().ToString(), InOutForItemToDrop.DropItemLevel);
			return;
		}
	
		RollSkillImplicits(InOutForItemToDrop, SkillImplicitAffixes);
		RollImplicit(InOutForItemToDrop, ImplicitAffixes);
		RollAffixesAndPrefixes(InOutForItemToDrop, PrefixAffixes, SuffixAffixes);
	}
	else if (InOutForItemToDrop.DropRarity == EObsidianItemRarity::Normal)
	{
		TArray<FObsidianDynamicItemAffix> SkillImplicitAffixes;
		TArray<FObsidianDynamicItemAffix> ImplicitAffixes;
		const bool bGatheredAffixes = CachedItemDataLoader->GetAllAffixesUpToQualityForCategory_NormalItemGeneration(
			InOutForItemToDrop.DropItemLevel, InDropItemCategory, InDropItemBaseTypeTag,
			ImplicitAffixes, SkillImplicitAffixes);
		if (bGatheredAffixes == false)
		{
			UE_LOG(ObLogItemDrop, Warning, TEXT("Could not find any Affixes for [%s] up to [%d] quality level."),
				*InDropItemCategory.GetTagName().ToString(), InOutForItemToDrop.DropItemLevel);
			return;
		}
	
		RollSkillImplicits(InOutForItemToDrop, SkillImplicitAffixes);
		RollImplicit(InOutForItemToDrop, ImplicitAffixes);
	}
}

void UObsidianItemDropComponent::HandleNoGeneration(FObsidianItemToDrop& InOutForItemToDrop,
	const UOInventoryItemFragment_Affixes* InAffixFragment)
{
	if (InAffixFragment == nullptr)
	{
		return;
	}

	TryToGivePrimaryItemAffix(InOutForItemToDrop, InAffixFragment);
	TryToGiveStaticImplicit(InOutForItemToDrop, InAffixFragment);
	
	if (FObsidianStaticItemAffix SkillImplicitAffix = InAffixFragment->GetStaticSkillImplicitAffix())
	{
		FObsidianActiveItemAffix ActiveAffix;
		ActiveAffix.InitializeWithStatic(SkillImplicitAffix, InOutForItemToDrop.DropItemLevel, InOutForItemToDrop.bShouldApplyMultiplier);
		InOutForItemToDrop.DropAffixes.Add(ActiveAffix);

		UE_LOG(ObLogItemDrop, VeryVerbose, TEXT("Adding Static Skill Implicit Affix: [%s], [%s]"), *SkillImplicitAffix.AffixTag.GetTagName().ToString(),
			*SkillImplicitAffix.AffixItemNameAddition);
	}
	
	for (const FObsidianStaticItemAffix& StaticAffix : InAffixFragment->GetStaticAffixes())
	{
		if (StaticAffix)
		{
			FObsidianActiveItemAffix ActiveAffix;
			ActiveAffix.InitializeWithStatic(StaticAffix, InOutForItemToDrop.DropItemLevel, InOutForItemToDrop.bShouldApplyMultiplier);
			InOutForItemToDrop.DropAffixes.Add(ActiveAffix);
						
			UE_LOG(ObLogItemDrop, VeryVerbose, TEXT("Adding Static Affix: [%s], [%s]"), *StaticAffix.AffixTag.GetTagName().ToString(),
				*StaticAffix.AffixItemNameAddition);
		}
	}
}

void UObsidianItemDropComponent::RollSkillImplicits(FObsidianItemToDrop& InOutForItemToDrop, const TArray<FObsidianDynamicItemAffix>& InSkillImplicits)
{
	if (InSkillImplicits.IsEmpty() == false)
	{
		FObsidianDynamicItemAffix RolledItemAffix = UObsidianItemsFunctionLibrary::GetRandomDynamicAffix(InSkillImplicits);
		FObsidianActiveItemAffix ActiveAffix;
		ActiveAffix.InitializeWithDynamic(RolledItemAffix, InOutForItemToDrop.DropItemLevel, InOutForItemToDrop.bShouldApplyMultiplier);
		InOutForItemToDrop.DropAffixes.Add(ActiveAffix);

		UE_LOG(ObLogItemDrop, VeryVerbose, TEXT("Adding Skill Implicit Affix: [%s], [%s]"), *RolledItemAffix.AffixTag.GetTagName().ToString(),
					*RolledItemAffix.AffixItemNameAddition);
	}
}

void UObsidianItemDropComponent::RollImplicit(FObsidianItemToDrop& InOutForItemToDrop, const TArray<FObsidianDynamicItemAffix>& InImplicits)
{
	if (InImplicits.IsEmpty() == false)
	{
		FObsidianDynamicItemAffix RolledItemAffix = UObsidianItemsFunctionLibrary::GetRandomDynamicAffix(InImplicits);
		FObsidianActiveItemAffix ActiveAffix;
		ActiveAffix.InitializeWithDynamic(RolledItemAffix, InOutForItemToDrop.DropItemLevel, InOutForItemToDrop.bShouldApplyMultiplier);
		InOutForItemToDrop.DropAffixes.Add(ActiveAffix);

		UE_LOG(ObLogItemDrop, VeryVerbose, TEXT("Adding Implicit Affix: [%s], [%s]"), *RolledItemAffix.AffixTag.GetTagName().ToString(),
					*RolledItemAffix.AffixItemNameAddition);
	}
}

void UObsidianItemDropComponent::RollAffixesAndPrefixes(FObsidianItemToDrop& InOutForItemToDrop, TArray<FObsidianDynamicItemAffix>& InOutPrefixes,
	TArray<FObsidianDynamicItemAffix>& InOutSuffixes)
{
	if (InOutPrefixes.IsEmpty() && InOutSuffixes.IsEmpty())
	{
		return;
	}
	
	const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>();
	if (ItemDataSettings == nullptr)
	{
		UE_LOG(ObLogItemData, Error, TEXT("ItemDataSettings was not found in [%hs]"), __FUNCTION__);
		return;
	}
	
	uint8 AffixCountToRoll = GetNumberOfAffixesToRollWeighted(InOutForItemToDrop.DropRarity);
	
	const uint8 MaxPrefixCount = ItemDataSettings->GetMaxPrefixCountForRarity(InOutForItemToDrop.DropRarity);
	const uint8 MaxSuffixCount = ItemDataSettings->GetMaxSuffixCountForRarity(InOutForItemToDrop.DropRarity);
	const uint8 MaximumNumberOfAffixesAvailableToAdd = FMath::Min<uint8>(MaxSuffixCount, InOutSuffixes.Num()) +
		FMath::Min<uint8>(MaxPrefixCount, InOutPrefixes.Num());
	if (AffixCountToRoll > MaximumNumberOfAffixesAvailableToAdd)
	{
		UE_LOG(ObLogItemDrop, Error, TEXT("Cannot safely add affixes: requested [%d], available [%d] (Prefixes [%d], Suffixes [%d]).\n"
		    "Falling back to maximum available count."),
			AffixCountToRoll, MaximumNumberOfAffixesAvailableToAdd, InOutPrefixes.Num(), InOutSuffixes.Num());
		AffixCountToRoll = MaximumNumberOfAffixesAvailableToAdd;
	}
	
	uint8 AddedPrefixes = 0;
	uint8 AddedSuffixes = 0;
	while (AddedPrefixes + AddedSuffixes < AffixCountToRoll)
	{
		bool bCanRollPrefix = !InOutPrefixes.IsEmpty() && (AddedPrefixes < MaxPrefixCount);
		bool bCanRollSuffix = !InOutSuffixes.IsEmpty() && (AddedSuffixes < MaxSuffixCount);
		if (bCanRollPrefix == false && bCanRollSuffix == false)
		{
			break;
		}
		
		const uint8 PrefixWeight = bCanRollPrefix ? MaxPrefixCount - AddedPrefixes : 0;
		const uint8 SuffixWeight = bCanRollSuffix ? MaxSuffixCount - AddedSuffixes : 0;
		bool bRollPrefix = FMath::FRandRange(0, (float)PrefixWeight + (float)SuffixWeight) < PrefixWeight;
		bRollPrefix = (bRollPrefix && bCanRollPrefix) || (!bRollPrefix && !bCanRollSuffix);
		if (bRollPrefix) 
		{
			FObsidianDynamicItemAffix RolledItemPrefix = UObsidianItemsFunctionLibrary::GetRandomDynamicAffix(InOutPrefixes);
			checkf(!InOutForItemToDrop.DropAffixes.Contains(RolledItemPrefix), TEXT("Item already contains this affix."));
			if (InOutForItemToDrop.DropAffixes.Contains(RolledItemPrefix)) // For shipping builds I don't want to crash but want to skip this affix.
			{
				UE_LOG(ObLogItemDrop, Warning, TEXT("Skipped duplicate affix [%s]."), *RolledItemPrefix.AffixTag.GetTagName().ToString());
				continue;
			}
			
			FObsidianActiveItemAffix ActiveAffix;
			ActiveAffix.InitializeWithDynamic(RolledItemPrefix, InOutForItemToDrop.DropItemLevel, InOutForItemToDrop.bShouldApplyMultiplier);
			InOutForItemToDrop.DropAffixes.Add(ActiveAffix);
			InOutPrefixes.Remove(RolledItemPrefix);
			++AddedPrefixes;
			
			UE_LOG(ObLogItemDrop, VeryVerbose, TEXT("Adding Prefix Affix: [%s], [%s]"), *RolledItemPrefix.AffixTag.GetTagName().ToString(),
				*RolledItemPrefix.AffixItemNameAddition);
		}
		else if (bCanRollSuffix)
		{
			FObsidianDynamicItemAffix RolledItemSuffix = UObsidianItemsFunctionLibrary::GetRandomDynamicAffix(InOutSuffixes);
			checkf(!InOutForItemToDrop.DropAffixes.Contains(RolledItemSuffix), TEXT("Item already contains this affix."));
			if (InOutForItemToDrop.DropAffixes.Contains(RolledItemSuffix)) // For shipping builds I don't want to crash but want to skip this affix.
			{
				UE_LOG(ObLogItemDrop, Warning, TEXT("Skipped duplicate affix [%s]."), *RolledItemSuffix.AffixTag.GetTagName().ToString());
				continue;
			}
			
			FObsidianActiveItemAffix ActiveAffix;
			ActiveAffix.InitializeWithDynamic(RolledItemSuffix, InOutForItemToDrop.DropItemLevel, InOutForItemToDrop.bShouldApplyMultiplier);
			InOutForItemToDrop.DropAffixes.Add(ActiveAffix);
			InOutSuffixes.Remove(RolledItemSuffix);
			++AddedSuffixes;
					
			UE_LOG(ObLogItemDrop, VeryVerbose, TEXT("Adding Suffix Affix: [%s], [%s]"), *RolledItemSuffix.AffixTag.GetTagName().ToString(),
				*RolledItemSuffix.AffixItemNameAddition);
		}
#if !UE_BUILD_SHIPPING
		else
		{
			UE_LOG(ObLogItemDrop, Error, TEXT("Error why trying to roll affixes, both roll prefix and roll suffix branch was not chosen. \n"
											"Item [%s],\n"
											"Rarity [%s],\n"
											"Affix to add in this operation [%d],\n"
											"Affixes already added [%d],\n"
											"Possible Prefixes to add [%d], already added Prefixes [%d]\n"
											"Possible Suffixes to add [%d], already added Suffixes [%d]\n"
											"Please make sure the logic is right."),
											*GetNameSafe(InOutForItemToDrop.ItemDefinitionClass), *DropComponentDebugHelpers::GetRarityDebugString(InOutForItemToDrop.DropRarity),
											AffixCountToRoll, AddedSuffixes + AddedPrefixes, InOutPrefixes.Num(), AddedPrefixes, InOutSuffixes.Num(), AddedSuffixes);
		}
		UE_LOG(ObLogItemDrop, VeryVerbose, TEXT("End of iteration AddedPrefixes [%d], AddedSuffixes: [%d], CountToReach: [%d]"), AddedPrefixes, AddedSuffixes, AffixCountToRoll);
#endif
	}
}

void UObsidianItemDropComponent::TryToGiveStaticImplicit(FObsidianItemToDrop& InOutForItemToDrop, const UOInventoryItemFragment_Affixes* InAffixFragment)
{
	if (InAffixFragment && InAffixFragment->HasImplicitAffix())
	{
		if (FObsidianStaticItemAffix StaticImplicitAffix = InAffixFragment->GetStaticImplicitAffix())
		{
			FObsidianActiveItemAffix ActiveAffix;
			ActiveAffix.InitializeWithStatic(StaticImplicitAffix, InOutForItemToDrop.DropItemLevel, InOutForItemToDrop.bShouldApplyMultiplier);
			InOutForItemToDrop.DropAffixes.Add(ActiveAffix);

			UE_LOG(ObLogItemDrop, VeryVerbose, TEXT("Adding Static Implicit Affix: [%s], [%s]"), *StaticImplicitAffix.AffixTag.GetTagName().ToString(),
				*StaticImplicitAffix.AffixItemNameAddition);
		}
	}
}

void UObsidianItemDropComponent::TryToGivePrimaryItemAffix(FObsidianItemToDrop& InOutForItemToDrop, const UOInventoryItemFragment_Affixes* InAffixFragment)
{
	if (InAffixFragment && InAffixFragment->HasPrimaryItemAffix())
	{
		TArray<FObsidianStaticItemAffix> PrimaryItemAffixes = InAffixFragment->GetPrimaryItemAffixes();
		for (const FObsidianStaticItemAffix& PrimaryAffix : PrimaryItemAffixes)
		{
			if (PrimaryAffix)
			{
				FObsidianActiveItemAffix ActiveAffix;
				ActiveAffix.InitializeWithStatic(PrimaryAffix, InOutForItemToDrop.DropItemLevel, InOutForItemToDrop.bShouldApplyMultiplier);
				InOutForItemToDrop.DropAffixes.Add(ActiveAffix);

				UE_LOG(ObLogItemDrop, VeryVerbose, TEXT("Adding Primary Item Affix: [%s], [%s]"), *PrimaryAffix.AffixTag.GetTagName().ToString(),
					*PrimaryAffix.AffixItemNameAddition);
			}
		}
	}
}

FGameplayTag UObsidianItemDropComponent::GetItemBaseTypeFromDropItem(const FObsidianDropItem& InDropItem)
{
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = InDropItem.SoftTreasureItemDefinitionClass.Get();
	if (ItemDef == nullptr)
	{
		ItemDef = InDropItem.SoftTreasureItemDefinitionClass.LoadSynchronous();
	}

	if (ItemDef)
	{
		if (const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
		{
			return ItemDefault->GetItemBaseTypeTag();
		}
	}
	
	return FGameplayTag::EmptyTag;
}

EObsidianItemRarity UObsidianItemDropComponent::GetItemDefaultRarityFromDropItem(const FObsidianDropItem& InDropItem)
{
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = InDropItem.SoftTreasureItemDefinitionClass.Get();
	if (ItemDef == nullptr)
	{
		ItemDef = InDropItem.SoftTreasureItemDefinitionClass.LoadSynchronous();
	}

	if (ItemDef)
	{
		if (const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
		{
			return ItemDefault->GetItemDefaultRarity();
		}
	}
	
	return EObsidianItemRarity::None;
}

bool UObsidianItemDropComponent::ShouldApplyAffixValueMultiplier(const EObsidianItemRarity InForItemRarity)
{
	if (InForItemRarity == EObsidianItemRarity::Magic)
	{
		return FMath::FRandRange(0.0f, 1.0f) >= 0.8f;
	}
	return false;
}

uint8 UObsidianItemDropComponent::GetNumberOfAffixesToRollWeighted(const EObsidianItemRarity InForItemRarity)
{
	const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>();
	if (ItemDataSettings == nullptr)
	{
		UE_LOG(ObLogItemData, Error, TEXT("ItemDataSettings was not found in [%hs]"), __FUNCTION__);
		return 0;
	}

	if (InForItemRarity != EObsidianItemRarity::Magic && InForItemRarity != EObsidianItemRarity::Rare)
	{
		return 0;
	}
	
	const uint8 MinAffixCount = ItemDataSettings->GetNaturalMinAffixCountForRarity(InForItemRarity);
	const uint8 MaxAffixCount = ItemDataSettings->GetMaxAffixCountForRarity(InForItemRarity);
	
	TArray<uint8> AffixValues;
	AffixValues.Reserve(MaxAffixCount - MinAffixCount + 1);
	TArray<uint8> ValuesWeights = ItemDataSettings->GetAffixNumberWeightsForRarity(InForItemRarity);
	
	int32 WeightIndex = 0;
	float TotalWeight = 0.0f;
	for (int32 i = MinAffixCount; i <= MaxAffixCount; i++)
	{
		AffixValues.Add(i);
		TotalWeight += ValuesWeights[WeightIndex];
		WeightIndex++;
	}
	
	check(AffixValues.Num() == ValuesWeights.Num());

	const float Random = FMath::FRandRange(0.0f, TotalWeight);

	float Cumulative = 0.0f;
	for (int32 i = 0; i < ValuesWeights.Num(); ++i)
	{
		Cumulative += ValuesWeights[i];
		if (Random <= Cumulative)
		{
			return AffixValues[i];
		}
	}

	check(false);
	return 0;
}

void UObsidianItemDropComponent::AdjustItemRequirementsBasedOnAddedAffixes(FObsidianItemRequirements& OutRequirements,
                                                                           const FObsidianItemToDrop& InFromItemToDrop)
{
	for (const FObsidianActiveItemAffix& Affix : InFromItemToDrop.DropAffixes)
	{
		check(Affix.CurrentAffixValue.IsValid());

		const int8 AffixMinLevelRequirement = Affix.CurrentAffixValue.AffixTier.MinItemLevelRequirement;
		if (AffixMinLevelRequirement > OutRequirements.RequiredLevel)
		{
			OutRequirements.RequiredLevel = AffixMinLevelRequirement;
		}

		//TODO(intrxx) Adjust the Attribute Magnitude requirements based on Affixes too?
	}

	OutRequirements.bInitialized = true;
	OutRequirements.bHasAnyRequirements = true;
}

void UObsidianItemDropComponent::GetTreasureClassesToRollFrom(const uint8 InMaxTreasureClassQuality, TArray<FObsidianTreasureClass>& OutTreasureClasses, TArray<FObsidianTreasureClass>& OutMustRollFromTreasureClasses)
{
	bool bRollFromCommonSet = true;
	for (const FObsidianAdditionalTreasureList& AdditionalTreasureList : AdditionalTreasureLists) 
	{
		UObsidianTreasureList* TreasureListToAdd = AdditionalTreasureList.TreasureList.Get();
		if (TreasureListToAdd == nullptr)
		{
			TreasureListToAdd = AdditionalTreasureList.TreasureList.LoadSynchronous();
			UE_LOG(ObLogItemDrop, Warning, TEXT("AdditionalTreasureList wasn't loaded correctly and needed to be loaded Synchronously."));
			if (TreasureListToAdd)
			{
				UE_LOG(ObLogItemDrop, Error, TEXT("AdditionalTreasureList was invalid in [%hs]."), __FUNCDNAME__);
				continue;
			}
		}

		const EObsidianAdditionalTreasureListPolicy Policy = AdditionalTreasureList.TreasureListPolicy;
		if (Policy == EObsidianAdditionalTreasureListPolicy::OverrideRoll)
		{
			TArray<FObsidianTreasureClass> ClassesToAdd = TreasureListToAdd->GetAllTreasureClasses();
			if (ClassesToAdd.IsEmpty() == false)
			{
				// This is safe as the AdditionalTreasureLists have strict Data Validation
				OutTreasureClasses.Append(ClassesToAdd);
				bRollFromCommonSet = false;
				continue;
			}
			UE_LOG(ObLogItemDrop, Error, TEXT("AdditionalTreasureLists contains List with OverrideRoll Policy but is empty."));
		}
			
		if (Policy == EObsidianAdditionalTreasureListPolicy::TryToRoll)
		{
			OutTreasureClasses.Append(TreasureListToAdd->GetAllTreasureClassesUpToQuality(InMaxTreasureClassQuality));
		}
		else if (Policy == EObsidianAdditionalTreasureListPolicy::TryToAddAlwaysRoll)
		{
			OutMustRollFromTreasureClasses.Append(TreasureListToAdd->GetAllTreasureClassesUpToQuality(InMaxTreasureClassQuality));
		}
		else if (Policy == EObsidianAdditionalTreasureListPolicy::AlwaysRoll)
		{
			OutMustRollFromTreasureClasses.Append(TreasureListToAdd->GetAllTreasureClasses());
		}
	}

	if (bRollFromCommonSet == false)
	{
		return;
	}
	
	if (bLimitCommonTreasureCategory && LimitCommonTreasureCategoryTag.IsValid())
	{
		ensureMsgf(CachedItemDataLoader->GetAllCommonTreasureClassesUpToQualityForCategory(InMaxTreasureClassQuality, OutTreasureClasses,
			LimitCommonTreasureCategoryTag), TEXT("Gathering TreasureClasses failed in [%hs]."), __FUNCTION__);
	}
	else
	{
		ensureMsgf(CachedItemDataLoader->GetAllCommonTreasureClassesUpToQuality(InMaxTreasureClassQuality, OutTreasureClasses),
			TEXT("Gathering TreasureClasses failed in [%hs]."), __FUNCTION__);
	}
}

FTransform UObsidianItemDropComponent::GetDropTransformAligned(const AActor* InDroppingActor, const FVector& InOverrideDropLocation) const
{
	FTransform InvalidTransform = FTransform::Identity;
	
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return InvalidTransform;
	}
	
	if (InDroppingActor == nullptr)
	{
		UE_LOG(ObLogItemDrop, Error, TEXT("DroppingActor is null in [%hs]"), __FUNCTION__);
		return InvalidTransform;
	}

	FVector DropLocation = FVector::ZeroVector;
	if (InOverrideDropLocation == FVector::ZeroVector)
	{
		const FVector OwnerLocation = InDroppingActor->GetActorLocation();
		DropLocation = OwnerLocation;

		FNavLocation RandomPointLocation;
		const UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(World);
		if (NavigationSystem && NavigationSystem->GetRandomPointInNavigableRadius(OwnerLocation, ItemDropRadius, RandomPointLocation))
		{
			DropLocation = RandomPointLocation.Location;
		}
		else
		{
			UE_LOG(ObLogItemDrop, Warning, TEXT("Could not find a navigable drop location around [%s], dropping the item at its location."),
				*GetNameSafe(InDroppingActor));
		}
	}
	else
	{
		DropLocation = InOverrideDropLocation;
	}

	FHitResult GroundTraceResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(InDroppingActor);
	const FVector GroundTraceEndLocation = FVector(DropLocation.X, DropLocation.Y, DropLocation.Z - 300.0f);
	World->LineTraceSingleByChannel(GroundTraceResult, DropLocation, GroundTraceEndLocation, ECC_Visibility, QueryParams);

	FRotator ItemRotation = FRotator::ZeroRotator;
	if(GroundTraceResult.bBlockingHit) // We are able to align the item to the ground better
	{
		FVector RandomisedRotationVector = FMath::VRand().GetSafeNormal();
		ItemRotation = UKismetMathLibrary::MakeRotFromZY(GroundTraceResult.ImpactNormal, RandomisedRotationVector);
		DropLocation = GroundTraceResult.Location;
	}

	return FTransform(ItemRotation, DropLocation, FVector(1.0f, 1.0f, 1.0f));
}

EObsidianItemRarity UObsidianItemDropComponent::RollItemRarity(const EObsidianItemRarity InMaxRarity)
{
	const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>();
	if (ItemDataSettings == nullptr)
	{
		UE_LOG(ObLogItemData, Error, TEXT("ItemDataSettings was not found in [%hs]"), __FUNCTION__);
		return EObsidianItemRarity::Normal;
	}
	
	TMap<EObsidianItemRarity, uint16> RarityToWeightMap;
	for (const TPair<EObsidianItemRarity, uint16>& RarityWithWeight : ItemDataSettings->DefaultRarityToWeightMap)
	{
		if (RarityWithWeight.Key <= InMaxRarity)
		{
			RarityToWeightMap.Add(RarityWithWeight);
		}
	}
	
	uint32 MaxWeight = 0;
	for (const TPair<EObsidianItemRarity, uint16>& RarityWithWeight : RarityToWeightMap)
	{
		MaxWeight += RarityWithWeight.Value;
	}
	
	const uint32 Roll = FMath::RandRange(0, MaxWeight);
	uint32 Cumulative = 0;
	for (const TPair<EObsidianItemRarity, uint16>& RarityWithWeight : RarityToWeightMap)
	{
		Cumulative += RarityWithWeight.Value;
		if (Roll <= Cumulative)
		{
			return RarityWithWeight.Key;
		}
	}
	
	return EObsidianItemRarity::Normal;
}

#if WITH_EDITOR
EDataValidationResult UObsidianItemDropComponent::IsDataValid(FDataValidationContext& InContext) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(InContext), EDataValidationResult::Valid);

	uint16 TreasureClassesIndex = 0;
	TArray<EObsidianAdditionalTreasureListPolicy> Policies;
	bool bContainsOverridePolicy = false;
	for (const FObsidianAdditionalTreasureList& Class : AdditionalTreasureLists)
	{
		Result = CombineDataValidationResults(Result, Class.ValidateData(InContext, TreasureClassesIndex));
		EObsidianAdditionalTreasureListPolicy ClassPolicy = Class.TreasureListPolicy;
		if (ClassPolicy == EObsidianAdditionalTreasureListPolicy::OverrideRoll)
		{
			bContainsOverridePolicy = true;
		}
		Policies.Add(ClassPolicy);
		TreasureClassesIndex++;
	}

	if (bContainsOverridePolicy)
	{
		for (const EObsidianAdditionalTreasureListPolicy Policy : Policies)
		{
			if (Policy != EObsidianAdditionalTreasureListPolicy::OverrideRoll)
			{
				Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
				
				const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Additional Treasure Lists contains at least one different Policy (different than OverrideRoll) while containing OverrideRoll Policy! \n"
							"This is invalid and will lead to undefined behaviour, please make sure to change the setup")));

				InContext.AddError(ErrorMessage);
			}
		}
	}
	
	return Result;
}
#endif




// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemDrop/ObsidianItemDropComponent.h"

#include <NavigationSystem.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#if WITH_EDITOR
#include <Misc/DataValidation.h>
#endif

#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataLoaderSubsystem.h"
#include "InventoryItems/ItemDrop/ObsidianItemManagerSubsystem.h"
#include "InventoryItems/ItemDrop/ObsidianTreasureList.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"

DEFINE_LOG_CATEGORY(LogDropComponent);

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

	inline FString GetRarityDebugString(const EObsidianItemRarity Rarity)
	{
		return ItemRarityToDebugStringMap[Rarity];
	}
}

// ~ FObsidianAdditionalTreasureList

#if WITH_EDITOR
EDataValidationResult FObsidianAdditionalTreasureList::ValidateData(FDataValidationContext& Context, const int Index) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;
	
	if(TreasureList.IsNull())
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Treasure List at index [%i] is empty! \n"
			"Please fill Treasure List or delete this index entry in Additional Treasure Lists"), Index));

		Context.AddError(ErrorMessage);
	}
	
	return Result;
}
#endif

// ~ End of FObsidianAdditionalTreasureList

UObsidianItemDropComponent::UObsidianItemDropComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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

void UObsidianItemDropComponent::DropItems(const EObsidianEntityRarity DroppingEntityRarity, const uint8 DroppingEntityLevel,
	const FVector& InOverrideDropLocation)
{
	checkf(DroppingEntityRarity != EObsidianEntityRarity::None, TEXT("Entity Rarity passed to DropItems is None, setup or run time logic is invalid."));
	
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>();
	if (ItemDataSettings == nullptr)
	{
		UE_LOG(LogItemDataLoader, Error, TEXT("ItemDataSettings was not found in [%hs]"), ANSI_TO_TCHAR(__FUNCDNAME__));
		return;
	}

	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(World);
	if (GameInstance == nullptr)
	{
		UE_LOG(LogDropComponent, Error, TEXT("GameInstance is nullptr in [%hs]"), ANSI_TO_TCHAR(__FUNCDNAME__));
		return;
	}

	CachedItemDataLoader = CachedItemDataLoader == nullptr ? GameInstance->GetSubsystem<UObsidianItemDataLoaderSubsystem>() : CachedItemDataLoader;
	if (CachedItemDataLoader == nullptr)
	{
		UE_LOG(LogDropComponent, Error, TEXT("CachedItemDataLoader is nullptr in [%hs]"), ANSI_TO_TCHAR(__FUNCDNAME__));
		return;
	}
	
	const uint8 TreasureQuality = FMath::Clamp(
		(DroppingEntityLevel + ItemDataSettings->GetDefaultAddedTreasureQualityForEntityRarity(DroppingEntityRarity)),
		1, ItemDataSettings->MaxTreasureQuality);
	
	TArray<FObsidianTreasureClass> TreasureClasses;
	TArray<FObsidianTreasureClass> MustRollFromTreasureClasses;
	GetTreasureClassesToRollFrom(TreasureQuality, TreasureClasses, MustRollFromTreasureClasses);
	
	if (TreasureClasses.IsEmpty() && MustRollFromTreasureClasses.IsEmpty())
	{
		UE_LOG(LogDropComponent, Warning, TEXT("TreasureClasses are empty after getting them from both the common"
									   " set and additional lists, is enemy level to low for drops or something is broken?"));
		return;
	}
	
	uint8 DropRolls = ItemDataSettings->GetDefaultDropRollNumberForEntityRarity(DroppingEntityRarity);
	
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
		UE_LOG(LogDropComponent, Display, TEXT("No Items Were Rolled To Drop."));
		OnDroppingItemsFinishedDelegate.Broadcast(false);
		return;
	}
	
	if (const UObsidianItemManagerSubsystem* ManagerSubsystem = World->GetSubsystem<UObsidianItemManagerSubsystem>())
	{
		ManagerSubsystem->RequestDroppingItems(MoveTemp(ItemsToDrop));
		OnDroppingItemsFinishedDelegate.Broadcast(true);
	}
}

bool UObsidianItemDropComponent::ConstructItemToDrop(const FObsidianDropItem& DropItem, const FVector& InOverrideDropLocation, const uint8 TreasureQuality, FObsidianItemToDrop& OutItemToDrop)
{
	if (DropItem.IsValid() == false)
	{
		return false;
	}
	
	const AActor* OwningActor = GetOwner();
	if (OwningActor == nullptr)
	{
		UE_LOG(LogDropComponent, Error, TEXT("OwningActor of ItemDropComponent is null in [%hs]"),
			ANSI_TO_TCHAR(__FUNCTION__));
		return false;
	}

	TSoftClassPtr<UObsidianInventoryItemDefinition> ItemSoftItemDefinition = DropItem.SoftTreasureItemDefinitionClass;
	const EObsidianItemRarity RolledRarity = DropItem.bShouldRandomizeRarity ? RollItemRarity(DropItem.ItemMaxRarity) : EObsidianItemRarity::None;
	if (RolledRarity >= EObsidianItemRarity::Unique)
	{
		FGameplayTag ItemBaseTypeTag = DropItem.ItemBaseType;
		if (DropItem.ItemBaseType.IsValid() == false)
		{
			ItemBaseTypeTag = GetItemBaseTypeFromDropItem(DropItem);
		}
		
		if (CachedItemDataLoader && ItemBaseTypeTag.IsValid())
		{
			FObsidianTreasureClass SpecialItemsTreasureClass;
			CachedItemDataLoader->GetAllUniqueOrSetItemsOfBaseItemTypeUpToQuality(TreasureQuality, RolledRarity,
				ItemBaseTypeTag, SpecialItemsTreasureClass);
			
			const FObsidianDropItem RolledSpecialItem = SpecialItemsTreasureClass.GetRandomItemFromClass();
			ItemSoftItemDefinition = RolledSpecialItem.SoftTreasureItemDefinitionClass;
		}
	}
	
	OutItemToDrop.ItemDefinitionClass = ItemSoftItemDefinition.Get();
	if (OutItemToDrop.ItemDefinitionClass == nullptr)
	{
		UE_LOG(LogDropComponent, Warning, TEXT("ItemToDrop was not loaded previously, falling back to LoadSynchronous in [%hs]."),
			ANSI_TO_TCHAR(__FUNCTION__));
		OutItemToDrop.ItemDefinitionClass = ItemSoftItemDefinition.LoadSynchronous();
	}
	
	OutItemToDrop.DropRarity = DropItem.bShouldRandomizeRarity ? RolledRarity : GetItemDefaultRarityFromDropItem(DropItem);
	OutItemToDrop.bShouldApplyMultiplier = ShouldApplyAffixValueMultiplier(OutItemToDrop.DropRarity);
	OutItemToDrop.DropTransform = GetDropTransformAligned(OwningActor, InOverrideDropLocation);
	OutItemToDrop.DropStacks = DropItem.GetRandomStackSizeToDropAdjusted(TreasureQuality);
	GenerateItem(OutItemToDrop, TreasureQuality);
	return true;
}

FGameplayTag UObsidianItemDropComponent::GetItemBaseTypeFromDropItem(const FObsidianDropItem& DropItem)
{
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DropItem.SoftTreasureItemDefinitionClass.Get();
	if (ItemDef == nullptr)
	{
		ItemDef = DropItem.SoftTreasureItemDefinitionClass.LoadSynchronous();
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

EObsidianItemRarity UObsidianItemDropComponent::GetItemDefaultRarityFromDropItem(const FObsidianDropItem& DropItem)
{
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DropItem.SoftTreasureItemDefinitionClass.Get();
	if (ItemDef == nullptr)
	{
		ItemDef = DropItem.SoftTreasureItemDefinitionClass.LoadSynchronous();
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

bool UObsidianItemDropComponent::ShouldApplyAffixValueMultiplier(const EObsidianItemRarity ForItemRarity)
{
	if (ForItemRarity == EObsidianItemRarity::Magic)
	{
		return FMath::FRandRange(0.0f, 1.0f) >= 0.8f;
	}
	return false;
}

void UObsidianItemDropComponent::GenerateItem(FObsidianItemToDrop& ForItemToDrop, const uint8 MaxTreasureClassQuality)
{
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = ForItemToDrop.ItemDefinitionClass;
	if (ItemDef == nullptr)
	{
		UE_LOG(LogDropComponent, Error, TEXT("ItemDef is invalid in [%hs]."), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if (DefaultObject == nullptr)
	{
		UE_LOG(LogDropComponent, Error, TEXT("DefaultObject of SoftTreasureItemDefinitionClass is invalid,"
									   " abandoning [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	const UOInventoryItemFragment_Affixes* AffixFragment = Cast<UOInventoryItemFragment_Affixes>(
		DefaultObject->FindFragmentByClass(UOInventoryItemFragment_Affixes::StaticClass()));
	if (AffixFragment == nullptr)
	{
		//NOTE(intrxx) there is no Affix Fragment so there is nothing more to generate.
		return;		
	}

	if (ForItemToDrop.DropRarity == EObsidianItemRarity::Rare)
	{
		ForItemToDrop.DropRareItemDisplayNameAddition = CachedItemDataLoader->GetRandomRareItemNameAddition(MaxTreasureClassQuality,
			DefaultObject->GetItemCategoryTag());
	}
	else if (ForItemToDrop.bShouldApplyMultiplier) // Only magic items can apply affix multiplier, that's why it's else if.
	{
		ForItemToDrop.DropMagicItemDisplayNameAddition = CachedItemDataLoader->GetAffixMultiplierMagicItemNameAddition();
	}
	
	switch (AffixFragment->GetGenerationType())
	{
		case EObsidianAffixGenerationType::DefaultGeneration:
			{
				HandleDefaultGeneration(ForItemToDrop, DefaultObject->GetItemCategoryTag(), AffixFragment, MaxTreasureClassQuality);
			} break;
		case EObsidianAffixGenerationType::FullGeneration:
			{
				HandleFullGeneration(ForItemToDrop, DefaultObject->GetItemCategoryTag(), MaxTreasureClassQuality);
			} break;
		case EObsidianAffixGenerationType::NoGeneration:
			{
				HandleNoGeneration(ForItemToDrop, AffixFragment, MaxTreasureClassQuality);
			} break;
			default:
			{} break;
	}
}

void UObsidianItemDropComponent::HandleDefaultGeneration(FObsidianItemToDrop& ForItemToDrop, const FGameplayTag& DropItemCategory,
	const UOInventoryItemFragment_Affixes* AffixFragment, const uint8 MaxTreasureClassQuality)
{
	if (AffixFragment->HasImplicitAffix())
	{
		FObsidianStaticItemAffix ImplicitAffix = AffixFragment->GetStaticImplicitAffix();
		
		FObsidianActiveItemAffix ActiveAffix;
		ActiveAffix.InitializeWithStatic(ImplicitAffix, MaxTreasureClassQuality, ForItemToDrop.bShouldApplyMultiplier);
		ForItemToDrop.DropAffixes.Add(ActiveAffix);

		UE_LOG(LogTemp, Display, TEXT("Adding Implicit Affix: [%s], [%s]"), *ImplicitAffix.AffixTag.GetTagName().ToString(),
			*ImplicitAffix.AffixItemNameAddition);
	}
	
	if (ForItemToDrop.DropRarity != EObsidianItemRarity::Normal)
	{
		TArray<FObsidianDynamicItemAffix> PrefixAffixes;
		TArray<FObsidianDynamicItemAffix> SuffixAffixes;
		TArray<FObsidianDynamicItemAffix> SkillImplicitAffixes;
		const bool bGatheredAffixes = CachedItemDataLoader->GetAllAffixesUpToQualityForCategory_DefaultGeneration(MaxTreasureClassQuality,
			DropItemCategory, PrefixAffixes, SuffixAffixes, SkillImplicitAffixes);
		if (bGatheredAffixes == false)
		{
			UE_LOG(LogDropComponent, Warning, TEXT("Could not find any Affixes for [%s] up to [%d] quality level."),
				*DropItemCategory.GetTagName().ToString(), MaxTreasureClassQuality);
			return;
		}

		RollSkillImplicits(ForItemToDrop, SkillImplicitAffixes, MaxTreasureClassQuality);
		RollAffixesAndPrefixes(ForItemToDrop, PrefixAffixes, SuffixAffixes, MaxTreasureClassQuality);
	}
	else if (ForItemToDrop.DropRarity == EObsidianItemRarity::Normal)
	{
		TArray<FObsidianDynamicItemAffix> SkillImplicitAffixes;
		const bool bGatheredAffixes = CachedItemDataLoader->GetAllSkillImplicitsUpToQualityForCategory(MaxTreasureClassQuality,
			DropItemCategory, SkillImplicitAffixes);
		if (bGatheredAffixes == false)
		{
			UE_LOG(LogDropComponent, Warning, TEXT("Could not find any Skill Implicits for [%s] up to [%d] quality level."),
				*DropItemCategory.GetTagName().ToString(), MaxTreasureClassQuality);
			return;
		}

		RollSkillImplicits(ForItemToDrop, SkillImplicitAffixes, MaxTreasureClassQuality);
	}
}

void UObsidianItemDropComponent::HandleFullGeneration(FObsidianItemToDrop& ForItemToDrop, const FGameplayTag& DropItemCategory,
	const uint8 MaxTreasureClassQuality)
{
	if (ForItemToDrop.DropRarity != EObsidianItemRarity::Normal)
	{
		TArray<FObsidianDynamicItemAffix> ImplicitAffixes;
		TArray<FObsidianDynamicItemAffix> PrefixAffixes;
		TArray<FObsidianDynamicItemAffix> SuffixAffixes;
		TArray<FObsidianDynamicItemAffix> SkillImplicitAffixes;
		const bool bGatheredAffixes = CachedItemDataLoader->GetAllAffixesUpToQualityForCategory_FullGeneration(MaxTreasureClassQuality,
			DropItemCategory, PrefixAffixes, SuffixAffixes, ImplicitAffixes, SkillImplicitAffixes);
		if (bGatheredAffixes == false)
		{
			UE_LOG(LogDropComponent, Warning, TEXT("Could not find any Affixes for [%s] up to [%d] quality level."),
				*DropItemCategory.GetTagName().ToString(), MaxTreasureClassQuality);
			return;
		}
	
		RollSkillImplicits(ForItemToDrop, SkillImplicitAffixes, MaxTreasureClassQuality);
		RollImplicit(ForItemToDrop, ImplicitAffixes, MaxTreasureClassQuality);
		RollAffixesAndPrefixes(ForItemToDrop, PrefixAffixes, SuffixAffixes, MaxTreasureClassQuality);
	}
	else if (ForItemToDrop.DropRarity == EObsidianItemRarity::Normal)
	{
		TArray<FObsidianDynamicItemAffix> SkillImplicitAffixes;
		TArray<FObsidianDynamicItemAffix> ImplicitAffixes;
		const bool bGatheredAffixes = CachedItemDataLoader->GetAllAffixesUpToQualityForCategory_NormalItemGeneration(MaxTreasureClassQuality,
			DropItemCategory, ImplicitAffixes, SkillImplicitAffixes);
		if (bGatheredAffixes == false)
		{
			UE_LOG(LogDropComponent, Warning, TEXT("Could not find any Affixes for [%s] up to [%d] quality level."),
				*DropItemCategory.GetTagName().ToString(), MaxTreasureClassQuality);
			return;
		}
	
		RollSkillImplicits(ForItemToDrop, SkillImplicitAffixes, MaxTreasureClassQuality);
		RollImplicit(ForItemToDrop, ImplicitAffixes, MaxTreasureClassQuality);
	}
}

void UObsidianItemDropComponent::HandleNoGeneration(FObsidianItemToDrop& ForItemToDrop, const UOInventoryItemFragment_Affixes* AffixFragment,
	const uint8 MaxTreasureClassQuality)
{
	if (FObsidianStaticItemAffix SkillImplicitAffix = AffixFragment->GetStaticSkillImplicitAffix())
	{
		FObsidianActiveItemAffix ActiveAffix;
		ActiveAffix.InitializeWithStatic(SkillImplicitAffix, MaxTreasureClassQuality, ForItemToDrop.bShouldApplyMultiplier);
		ForItemToDrop.DropAffixes.Add(ActiveAffix);

		UE_LOG(LogTemp, Display, TEXT("Adding Static Skill Implicit Affix: [%s], [%s]"), *SkillImplicitAffix.AffixTag.GetTagName().ToString(),
			*SkillImplicitAffix.AffixItemNameAddition);
	}
				
	if (AffixFragment->HasImplicitAffix())
	{
		if (FObsidianStaticItemAffix StaticImplicitAffix = AffixFragment->GetStaticImplicitAffix())
		{
			FObsidianActiveItemAffix ActiveAffix;
			ActiveAffix.InitializeWithStatic(StaticImplicitAffix, MaxTreasureClassQuality, ForItemToDrop.bShouldApplyMultiplier);
			ForItemToDrop.DropAffixes.Add(ActiveAffix);

			UE_LOG(LogTemp, Display, TEXT("Adding Static Implicit Affix: [%s], [%s]"), *StaticImplicitAffix.AffixTag.GetTagName().ToString(),
				*StaticImplicitAffix.AffixItemNameAddition);
		}
	}
				
	for (const FObsidianStaticItemAffix& StaticAffix : AffixFragment->GetStaticAffixes())
	{
		if (StaticAffix)
		{
			FObsidianActiveItemAffix ActiveAffix;
			ActiveAffix.InitializeWithStatic(StaticAffix, MaxTreasureClassQuality, ForItemToDrop.bShouldApplyMultiplier);
			ForItemToDrop.DropAffixes.Add(ActiveAffix);
						
			UE_LOG(LogTemp, Display, TEXT("Adding Static Affix: [%s], [%s]"), *StaticAffix.AffixTag.GetTagName().ToString(),
				*StaticAffix.AffixItemNameAddition);
		}
	}
}

void UObsidianItemDropComponent::RollSkillImplicits(FObsidianItemToDrop& ForItemToDrop, const TArray<FObsidianDynamicItemAffix>& SkillImplicits, const uint8 MaxTreasureClassQuality)
{
	if (SkillImplicits.IsEmpty() == false)
	{
		FObsidianDynamicItemAffix RolledItemAffix = UObsidianItemsFunctionLibrary::GetRandomDynamicAffix(SkillImplicits);
		FObsidianActiveItemAffix ActiveAffix;
		ActiveAffix.InitializeWithDynamic(RolledItemAffix, MaxTreasureClassQuality, ForItemToDrop.bShouldApplyMultiplier);
		ForItemToDrop.DropAffixes.Add(ActiveAffix);

		UE_LOG(LogTemp, Display, TEXT("Adding Skill Implicit Affix: [%s], [%s]"), *RolledItemAffix.AffixTag.GetTagName().ToString(),
					*RolledItemAffix.AffixItemNameAddition);
	}
}

void UObsidianItemDropComponent::RollImplicit(FObsidianItemToDrop& ForItemToDrop, const TArray<FObsidianDynamicItemAffix>& Implicits,
	const uint8 MaxTreasureClassQuality)
{
	if (Implicits.IsEmpty() == false)
	{
		FObsidianDynamicItemAffix RolledItemAffix = UObsidianItemsFunctionLibrary::GetRandomDynamicAffix(Implicits);
		FObsidianActiveItemAffix ActiveAffix;
		ActiveAffix.InitializeWithDynamic(RolledItemAffix, MaxTreasureClassQuality, ForItemToDrop.bShouldApplyMultiplier);
		ForItemToDrop.DropAffixes.Add(ActiveAffix);

		UE_LOG(LogTemp, Display, TEXT("Adding Implicit Affix: [%s], [%s]"), *RolledItemAffix.AffixTag.GetTagName().ToString(),
					*RolledItemAffix.AffixItemNameAddition);
	}
}

void UObsidianItemDropComponent::RollAffixesAndPrefixes(FObsidianItemToDrop& ForItemToDrop, TArray<FObsidianDynamicItemAffix>& Prefixes,
	TArray<FObsidianDynamicItemAffix>& Suffixes, const uint8 MaxTreasureClassQuality)
{
	if (Prefixes.IsEmpty() && Suffixes.IsEmpty())
	{
		return;
	}
	
	const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>();
	if (ItemDataSettings == nullptr)
	{
		UE_LOG(LogItemDataLoader, Error, TEXT("ItemDataSettings was not found in [%hs]"), ANSI_TO_TCHAR(__FUNCDNAME__));
		return;
	}
	
	const uint8 MinAffixCount = ItemDataSettings->GetNaturalMinAffixCountForRarity(ForItemToDrop.DropRarity);
	const uint8 MaxAffixCount = ItemDataSettings->GetMaxAffixCountForRarity(ForItemToDrop.DropRarity);

	//TODO(intrxx) should this roll be weighted?
	uint8 AffixCountToRoll = FMath::RandRange(MinAffixCount, MaxAffixCount);
	
	const uint8 MaxPrefixCount = ItemDataSettings->GetMaxPrefixCountForRarity(ForItemToDrop.DropRarity);
	const uint8 MaxSuffixCount = ItemDataSettings->GetMaxSuffixCountForRarity(ForItemToDrop.DropRarity);
	const uint8 MaximumNumberOfAffixesAvailableToAdd = FMath::Min<uint8>(MaxSuffixCount, Suffixes.Num()) + FMath::Min<uint8>(MaxPrefixCount, Prefixes.Num());
	if (AffixCountToRoll > MaximumNumberOfAffixesAvailableToAdd)
	{
		UE_LOG(LogDropComponent, Error, TEXT("Cannot safely add affixes: requested [%d], available [%d] (Prefixes [%d], Suffixes [%d]).\n"
		    "Falling back to maximum available count."),
			AffixCountToRoll, MaximumNumberOfAffixesAvailableToAdd, Prefixes.Num(), Suffixes.Num());
		AffixCountToRoll = MaximumNumberOfAffixesAvailableToAdd;
	}
	
	uint8 AddedPrefixes = 0;
	uint8 AddedSuffixes = 0;
	while (AddedPrefixes + AddedSuffixes < AffixCountToRoll)
	{
		bool bCanRollPrefix = !Prefixes.IsEmpty() && (AddedPrefixes < MaxPrefixCount);
		bool bCanRollSuffix = !Suffixes.IsEmpty() && (AddedSuffixes < MaxSuffixCount);
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
			FObsidianDynamicItemAffix RolledItemPrefix = UObsidianItemsFunctionLibrary::GetRandomDynamicAffix(Prefixes);
			checkf(!ForItemToDrop.DropAffixes.Contains(RolledItemPrefix), TEXT("Item already contains this affix."));
			if (ForItemToDrop.DropAffixes.Contains(RolledItemPrefix)) // For shipping builds I don't want to crash but want to skip this affix.
			{
				UE_LOG(LogDropComponent, Warning, TEXT("Skipped duplicate affix [%s]."), *RolledItemPrefix.AffixTag.GetTagName().ToString());
				continue;
			}
			
			FObsidianActiveItemAffix ActiveAffix;
			ActiveAffix.InitializeWithDynamic(RolledItemPrefix, MaxTreasureClassQuality, ForItemToDrop.bShouldApplyMultiplier);
			ForItemToDrop.DropAffixes.Add(ActiveAffix);
			Prefixes.Remove(RolledItemPrefix);
			++AddedPrefixes;
			
			UE_LOG(LogTemp, Display, TEXT("Adding Prefix Affix: [%s], [%s]"), *RolledItemPrefix.AffixTag.GetTagName().ToString(),
				*RolledItemPrefix.AffixItemNameAddition);
		}
		else if (bCanRollSuffix)
		{
			FObsidianDynamicItemAffix RolledItemSuffix = UObsidianItemsFunctionLibrary::GetRandomDynamicAffix(Suffixes);
			checkf(!ForItemToDrop.DropAffixes.Contains(RolledItemSuffix), TEXT("Item already contains this affix."));
			if (ForItemToDrop.DropAffixes.Contains(RolledItemSuffix)) // For shipping builds I don't want to crash but want to skip this affix.
			{
				UE_LOG(LogDropComponent, Warning, TEXT("Skipped duplicate affix [%s]."), *RolledItemSuffix.AffixTag.GetTagName().ToString());
				continue;
			}
			
			FObsidianActiveItemAffix ActiveAffix;
			ActiveAffix.InitializeWithDynamic(RolledItemSuffix, MaxTreasureClassQuality, ForItemToDrop.bShouldApplyMultiplier);
			ForItemToDrop.DropAffixes.Add(ActiveAffix);
			Suffixes.Remove(RolledItemSuffix);
			++AddedSuffixes;
					
			UE_LOG(LogTemp, Display, TEXT("Adding Suffix Affix: [%s], [%s]"), *RolledItemSuffix.AffixTag.GetTagName().ToString(),
				*RolledItemSuffix.AffixItemNameAddition);
		}
#if !UE_BUILD_SHIPPING
		else
		{
			UE_LOG(LogDropComponent, Error, TEXT("Error why trying to roll affixes, both roll prefix and roll suffix branch was not chosen. \n"
											"Item [%s],\n"
											"Rarity [%s],\n"
											"Affix to add in this operation [%d],\n"
											"Affixes already added [%d],\n"
											"Possible Prefixes to add [%d], already added Prefixes [%d]\n"
											"Possible Suffixes to add [%d], already added Suffixes [%d]\n"
											"Please make sure the logic is right."),
											*GetNameSafe(ForItemToDrop.ItemDefinitionClass), *DropComponentDebugHelpers::GetRarityDebugString(ForItemToDrop.DropRarity),
											AffixCountToRoll, AddedSuffixes + AddedPrefixes, Prefixes.Num(), AddedPrefixes, Suffixes.Num(), AddedSuffixes);
		}
		UE_LOG(LogTemp, Warning, TEXT("End of iteration AddedPrefixes [%d], AddedSuffixes: [%d], CountToReach: [%d]"), AddedPrefixes, AddedSuffixes, AffixCountToRoll);
#endif
	}
}

void UObsidianItemDropComponent::GetTreasureClassesToRollFrom(const uint8 MaxTreasureClassQuality, TArray<FObsidianTreasureClass>& OutTreasureClasses, TArray<FObsidianTreasureClass>& OutMustRollFromTreasureClasses)
{
	bool bRollFromCommonSet = true;
	for (const FObsidianAdditionalTreasureList& AdditionalTreasureList : AdditionalTreasureLists) 
	{
		UObsidianTreasureList* TreasureListToAdd = AdditionalTreasureList.TreasureList.Get();
		if (TreasureListToAdd == nullptr)
		{
			TreasureListToAdd = AdditionalTreasureList.TreasureList.LoadSynchronous();
			UE_LOG(LogDropComponent, Warning, TEXT("AdditionalTreasureList wasn't loaded correctly and needed to be loaded Synchronously."));
			if (TreasureListToAdd)
			{
				UE_LOG(LogDropComponent, Error, TEXT("AdditionalTreasureList was invalid in [%hs]."), __FUNCDNAME__);
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
			UE_LOG(LogDropComponent, Error, TEXT("AdditionalTreasureLists contains List with OverrideRoll Policy but is empty."));
		}
			
		if (Policy == EObsidianAdditionalTreasureListPolicy::TryToRoll)
		{
			OutTreasureClasses.Append(TreasureListToAdd->GetAllTreasureClassesUpToQuality(MaxTreasureClassQuality));
		}
		else if (Policy == EObsidianAdditionalTreasureListPolicy::TryToAddAlwaysRoll)
		{
			OutMustRollFromTreasureClasses.Append(TreasureListToAdd->GetAllTreasureClassesUpToQuality(MaxTreasureClassQuality));
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
	
	const bool bSuccess = CachedItemDataLoader->GetAllCommonTreasureClassesUpToQuality(MaxTreasureClassQuality, OutTreasureClasses);
	if (bSuccess == false)
	{
		UE_LOG(LogDropComponent, Error, TEXT("Gathering TreasureClasses failed in [%hs]."), ANSI_TO_TCHAR(__FUNCTION__));
	}
}

FTransform UObsidianItemDropComponent::GetDropTransformAligned(const AActor* DroppingActor, const FVector& InOverrideDropLocation) const
{
	FTransform InvalidTransform = FTransform::Identity;
	
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return InvalidTransform;
	}
	
	if (DroppingActor == nullptr)
	{
		UE_LOG(LogDropComponent, Error, TEXT("DroppingActor is null in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		return InvalidTransform;
	}

	FVector DropLocation = FVector::ZeroVector;
	if (InOverrideDropLocation == FVector::ZeroVector)
	{
		const FVector OwnerLocation = DroppingActor->GetActorLocation();
		UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(World);

		if(NavigationSystem == nullptr)
		{
			UE_LOG(LogDropComponent, Error, TEXT("NavigationSystem is null in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
			return InvalidTransform;
		}
		
		FNavLocation RandomPointLocation;
		const bool bFound = NavigationSystem->GetRandomPointInNavigableRadius(OwnerLocation, ItemDropRadius, RandomPointLocation);
		if (bFound == false)
		{
			//TODO Change the location to somewhere valid.
			UE_LOG(LogDropComponent, Error, TEXT("Could not initially find a valid drop location in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		}
		
		DropLocation = RandomPointLocation.Location;
	}
	else
	{
		DropLocation = InOverrideDropLocation;
	}

	FHitResult GroundTraceResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(DroppingActor);
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

EObsidianItemRarity UObsidianItemDropComponent::RollItemRarity(const EObsidianItemRarity MaxRarity)
{
	const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>();
	if (ItemDataSettings == nullptr)
	{
		UE_LOG(LogItemDataLoader, Error, TEXT("ItemDataSettings was not found in [%hs]"), ANSI_TO_TCHAR(__FUNCDNAME__));
		return EObsidianItemRarity::Normal;
	}
	
	TMap<EObsidianItemRarity, uint16> RarityToWeightMap;
	for (const TPair<EObsidianItemRarity, uint16>& RarityWithWeight : ItemDataSettings->DefaultRarityToWeightMap)
	{
		if (RarityWithWeight.Key <= MaxRarity)
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
EDataValidationResult UObsidianItemDropComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	uint16 TreasureClassesIndex = 0;
	TArray<EObsidianAdditionalTreasureListPolicy> Policies;
	bool bContainsOverridePolicy = false;
	for (const FObsidianAdditionalTreasureList& Class : AdditionalTreasureLists)
	{
		Result = CombineDataValidationResults(Result, Class.ValidateData(Context, TreasureClassesIndex));
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

				Context.AddError(ErrorMessage);
			}
		}
	}
	
	return Result;
}
#endif




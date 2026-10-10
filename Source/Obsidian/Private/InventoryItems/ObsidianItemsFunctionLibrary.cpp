// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ObsidianItemsFunctionLibrary.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "Core/ObsidianGameplayStatics.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Stacks.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataLoaderSubsystem.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "Obsidian/ObsidianLogCategories.h"


UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Obsidian_TwoHand, "TwoHand");

const UObsidianInventoryItemFragment* UObsidianItemsFunctionLibrary::FindItemDefinitionFragment(const TSubclassOf<UObsidianInventoryItemDefinition> InItemDef, const TSubclassOf<UObsidianInventoryItemFragment> InFragmentClass)
{
	if((InItemDef != nullptr) && (InFragmentClass != nullptr))
	{
		return GetDefault<UObsidianInventoryItemDefinition>(InItemDef)->FindFragmentByClass(InFragmentClass);
	}
	return nullptr;
}

bool UObsidianItemsFunctionLibrary::IsTheSameItem(const UObsidianInventoryItemInstance* InInstanceA, const UObsidianInventoryItemInstance* InInstanceB)
{
	if(InInstanceA == nullptr || InInstanceB == nullptr)
	{
		return false;
	}

	if(InInstanceA->GetItemDef().Get() == InInstanceB->GetItemDef().Get())
	{
		return true;
	}
	return false;
}

bool UObsidianItemsFunctionLibrary::IsTheSameItem_WithDef(const UObsidianInventoryItemInstance* InInstance, const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef)
{
	if(InInstance == nullptr || InItemDef == nullptr)
	{
		return false;
	}

	if(InInstance->GetItemDef().Get() == InItemDef.Get())
	{
		return true;
	}
	return false;
}

bool UObsidianItemsFunctionLibrary::GetItemStats(const AObsidianPlayerController* InOwnerPC, const UObsidianInventoryItemInstance* InItemInstance,
	FObsidianItemStats& OutItemStats)
{
	if(InItemInstance == nullptr)
	{
		return false;
	}
	
	if(InItemInstance->IsStackable())
	{
		OutItemStats.SetStacks(InItemInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current),
			 InItemInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Max));
	}

	OutItemStats.SetItemImage(InItemInstance->GetItemImage(), InItemInstance->GetItemGridSpan());
	OutItemStats.SetDisplayName(InItemInstance->GetItemDisplayName());
	OutItemStats.SetItemLevel(InItemInstance->GetItemLevel());
	OutItemStats.SetRareDisplayNameAddition(InItemInstance->GetRareItemDisplayNameAddition());
	OutItemStats.SetMagicDisplayNameAddition(InItemInstance->GetMagicAffixMultiplierItemDisplayNameAddition());
	OutItemStats.SetDescription(InItemInstance->GetItemDescription());
	OutItemStats.SetAdditionalDescription(InItemInstance->GetItemAdditionalDescription());

	OutItemStats.ItemRarity = InItemInstance->GetItemRarity();
	
	const bool bIdentified = InItemInstance->IsItemIdentified();
	OutItemStats.SetIdentified(bIdentified);
	if(bIdentified)
	{
		OutItemStats.SetAffixDescriptionRows(FormatItemAffixes(InItemInstance->GetAllItemAffixes()));
	}
	else
	{
		OutItemStats.SetAffixDescriptionRows(FormatUnidentifiedItemAffixes(InItemInstance->GetAllItemAffixes()));
	}

	if (InItemInstance->HasEquippingRequirements())
	{
		FObsidianItemRequirementsUIDescription RequirementsUIDescription;
		if (GenerateItemEquippingRequirementsAsUIDesc(InOwnerPC, InItemInstance->GetEquippingRequirements(), RequirementsUIDescription))
		{
			OutItemStats.SetItemEquippingRequirements(RequirementsUIDescription);
		}
	}
	
	return true;
}

bool UObsidianItemsFunctionLibrary::GetItemStats_WithDef(const AObsidianPlayerController* InOwnerPC, const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef,
	const FObsidianItemGeneratedData& InItemGeneratedData, FObsidianItemStats& OutItemStats)
{
	if(IsValid(InItemDef) == false || InOwnerPC == nullptr)
	{
		return false;
	}

	const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(InItemDef);
	if(ItemDefault == nullptr)
	{
		return false;
	}

	OutItemStats.SetItemLevel(InItemGeneratedData.ItemLevel);
	
	if(ItemDefault->IsStackable())
	{
		if(const UOInventoryItemFragment_Stacks* StacksFrag = Cast<UOInventoryItemFragment_Stacks>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Stacks::StaticClass())))
		{
			OutItemStats.SetStacks(InItemGeneratedData.GetStackCount(),
				 StacksFrag->GetItemStackNumberByTag(ObsidianGameplayTags::Item::StackCount::Max));
		}
	}

	if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
	{
		OutItemStats.SetItemImage(AppearanceFrag->GetItemImage(), AppearanceFrag->GetItemGridSpanFromDesc());
		OutItemStats.SetDisplayName(AppearanceFrag->GetItemDisplayName());
		OutItemStats.SetRareDisplayNameAddition(InItemGeneratedData.NameData.RareItemDisplayNameAddition);
		OutItemStats.SetMagicDisplayNameAddition(InItemGeneratedData.NameData.MagicItemDisplayNameAddition);
		OutItemStats.SetDescription(AppearanceFrag->GetItemDescription());
		OutItemStats.SetAdditionalDescription(AppearanceFrag->GetItemAdditionalDescription());
	}

	OutItemStats.ItemRarity = InItemGeneratedData.ItemRarity;
	
	const bool bIdentified = IsDefinitionIdentified(ItemDefault, InItemGeneratedData);
	OutItemStats.SetIdentified(bIdentified);
	if (bIdentified)
	{
		OutItemStats.SetAffixDescriptionRows(FormatItemAffixes(InItemGeneratedData.ItemAffixes));
	}

	if (HasEquippingRequirements(InItemGeneratedData.ItemEquippingRequirements))
	{
		FObsidianItemRequirementsUIDescription RequirementsUIDescription;
		if (GenerateItemEquippingRequirementsAsUIDesc(InOwnerPC, InItemGeneratedData.ItemEquippingRequirements, RequirementsUIDescription))
		{
			OutItemStats.SetItemEquippingRequirements(RequirementsUIDescription);
		}
	}
	
	return true;
}

bool UObsidianItemsFunctionLibrary::GenerateItemEquippingRequirementsAsUIDesc(const AObsidianPlayerController* InOwnerPC,
	const FObsidianItemRequirements& InRequirements, FObsidianItemRequirementsUIDescription& OutRequirementsUIDescription)
{
	if (InOwnerPC == nullptr)
	{
		return false;
	}
	
	if (InRequirements.bInitialized == false || InRequirements.bHasAnyRequirements == false)
	{
		return false;
	}
	
	OutRequirementsUIDescription.SetHeroClassRequirement(InRequirements.HeroClassRequirement, InOwnerPC->GetHeroClass());

	const AObsidianPlayerState* OwnerPS = InOwnerPC->GetObsidianPlayerState();
	if (OwnerPS == nullptr)
	{
		return false;
	}
	
	OutRequirementsUIDescription.SetHeroLevelRequirement(InRequirements.RequiredLevel, OwnerPS->GetHeroLevel());

	const UObsidianAbilitySystemComponent* OwnerASC = OwnerPS->GetObsidianAbilitySystemComponent();
	if (OwnerASC == nullptr)
	{
		return false;
	}

	for (const FObsidianAttributeRequirement& AttributeReq : InRequirements.AttributeRequirements)
	{
		OutRequirementsUIDescription.SetAttributeRequirement(AttributeReq.RequiredAttribute,
												AttributeReq.RequiredAttributeMagnitude,
												OwnerASC->GetNumericAttribute(AttributeReq.RequiredAttribute));
	}

	return true;
}

TArray<FObsidianAffixDescriptionRow> UObsidianItemsFunctionLibrary::FormatItemAffixes(
	const TArray<FObsidianActiveItemAffix>& InItemAffixes)
{
	TArray<FObsidianAffixDescriptionRow> AffixDescriptionRows;
	AffixDescriptionRows.Reserve(InItemAffixes.Num());
	
	for(const FObsidianActiveItemAffix& Affix : InItemAffixes)
	{
		check(Affix);
		FObsidianAffixDescriptionRow Row;
		Row.AffixTag = Affix.AffixTag;
		Row.AffixRowDescription = FText::FromString(Affix.ActiveAffixDescription);
		Row.AffixItemNameAddition = Affix.AffixItemNameAddition;
		Row.SetAffixAdditionalDescription(Affix.AffixType, Affix.GetCurrentAffixTier());
		AffixDescriptionRows.Add(Row);
	}
	return AffixDescriptionRows;
}

TArray<FObsidianAffixDescriptionRow> UObsidianItemsFunctionLibrary::FormatUnidentifiedItemAffixes(
	const TArray<FObsidianActiveItemAffix>& InItemAffixes)
{
	TArray<FObsidianAffixDescriptionRow> AffixDescriptionRows;
	
	for(const FObsidianActiveItemAffix& Affix : InItemAffixes)
	{
		check(Affix);
		if (Affix.AffixType == EObsidianAffixType::SkillImplicit || Affix.AffixType == EObsidianAffixType::PrimaryItemAffix)
		{
			FObsidianAffixDescriptionRow Row;
			Row.AffixTag = Affix.AffixTag;
			Row.AffixRowDescription = FText::FromString(Affix.ActiveAffixDescription);
			Row.AffixItemNameAddition = Affix.AffixItemNameAddition;
			Row.SetAffixAdditionalDescription(Affix.AffixType, Affix.GetCurrentAffixTier());
			AffixDescriptionRows.Add(Row);
		}
	}
	return AffixDescriptionRows;
}

bool UObsidianItemsFunctionLibrary::HasEquippingRequirements(const FObsidianItemRequirements& InRequirements)
{
	if (InRequirements.RequiredLevel > 0)
	{
		return true;
	}

	if (InRequirements.HeroClassRequirement > EObsidianHeroClass::None)
	{
		return true;
	}

	for (const FObsidianAttributeRequirement& AttributeReq : InRequirements.AttributeRequirements)
	{
		if (AttributeReq.RequiredAttributeMagnitude > 0)
		{
			return true;
		}
	}
	
	return false;
}

FObsidianDynamicItemAffix UObsidianItemsFunctionLibrary::GetRandomSkillImplicitForItem(
	const UObsidianInventoryItemInstance* InForItem)
{
	if (InForItem == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Provided Item is invalid in [%hs]"), __FUNCTION__);
		return FObsidianDynamicItemAffix();
	}

	const UWorld* World = InForItem->GetWorld();
	if (World == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Could not extract valid World from provided item [%s] in [%hs]"),
			*GetNameSafe(InForItem), __FUNCTION__);
		return FObsidianDynamicItemAffix();
	}

	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(World);
	if (GameInstance == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Could not get GameInstance in [%hs]"), __FUNCTION__);
		return FObsidianDynamicItemAffix();
	}
	
	if (UObsidianItemDataLoaderSubsystem* ItemDataLoader = GameInstance->GetSubsystem<UObsidianItemDataLoaderSubsystem>())
	{
		TArray<FObsidianDynamicItemAffix> SkillImplicits;
		ItemDataLoader->GetAllSkillImplicitsUpToQualityForCategory(InForItem->GetItemLevel(), InForItem->GetItemCategoryTag(),
			InForItem->GetItemBaseTypeTag(), /** OUT */ SkillImplicits);

		return GetRandomDynamicAffix(SkillImplicits);
	}

	UE_LOG(ObLogItems, Error, TEXT("ItemDataLoader is invalid in [%hs]"), __FUNCTION__);
	return FObsidianDynamicItemAffix();
}

FObsidianDynamicItemAffix UObsidianItemsFunctionLibrary::GetRandomImplicitForItem(
	const UObsidianInventoryItemInstance* InForItem)
{
	if (InForItem == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Provided Item is invalid in [%hs]"), __FUNCTION__);
		return FObsidianDynamicItemAffix();
	}

	const UWorld* World = InForItem->GetWorld();
	if (World == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Could not extract valid World from provided item [%s] in [%hs]"),
			*GetNameSafe(InForItem), __FUNCTION__);
		return FObsidianDynamicItemAffix();
	}

	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(World);
	if (GameInstance == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Could not get GameInstance in [%hs]"), __FUNCTION__);
		return FObsidianDynamicItemAffix();
	}
	
	if (UObsidianItemDataLoaderSubsystem* ItemDataLoader = GameInstance->GetSubsystem<UObsidianItemDataLoaderSubsystem>())
	{
		TArray<FObsidianDynamicItemAffix> Implicits;
		ItemDataLoader->GetAllImplicitsUpToQualityForCategory(InForItem->GetItemLevel(), InForItem->GetItemCategoryTag(),
			InForItem->GetItemBaseTypeTag(), /** OUT */ Implicits);

		return GetRandomDynamicAffix(Implicits);
	}

	UE_LOG(ObLogItems, Error, TEXT("ItemDataLoader is invalid in [%hs]"), __FUNCTION__);
	return FObsidianDynamicItemAffix();
}

FObsidianDynamicItemAffix UObsidianItemsFunctionLibrary::GetRandomPrefixForItem(
	const UObsidianInventoryItemInstance* InForItem)
{
	if (InForItem == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Provided Item is invalid in [%hs]"), __FUNCTION__);
		return FObsidianDynamicItemAffix();
	}

	const UWorld* World = InForItem->GetWorld();
	if (World == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Could not extract valid World from provided item [%s] in [%hs]"),
			*GetNameSafe(InForItem), __FUNCTION__);
		return FObsidianDynamicItemAffix();
	}

	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(World);
	if (GameInstance == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Could not get GameInstance in [%hs]"), __FUNCTION__);
		return FObsidianDynamicItemAffix();
	}
	
	if (UObsidianItemDataLoaderSubsystem* ItemDataLoader = GameInstance->GetSubsystem<UObsidianItemDataLoaderSubsystem>())
	{
		TArray<FObsidianDynamicItemAffix> Prefixes;
		ItemDataLoader->GetAllPrefixesUpToQualityForCategory(InForItem->GetItemLevel(), InForItem->GetItemCategoryTag(),
			InForItem->GetItemBaseTypeTag(), /** OUT */ Prefixes);

		return GetRandomDynamicAffix(Prefixes);
	}

	UE_LOG(ObLogItems, Error, TEXT("ItemDataLoader is invalid in [%hs]"), __FUNCTION__);
	return FObsidianDynamicItemAffix();
}

FObsidianDynamicItemAffix UObsidianItemsFunctionLibrary::GetRandomSuffixForItem(
	const UObsidianInventoryItemInstance* InForItem)
{
	if (InForItem == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Provided Item is invalid in [%hs]"), __FUNCTION__);
		return FObsidianDynamicItemAffix();
	}

	const UWorld* World = InForItem->GetWorld();
	if (World == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Could not extract valid World from provided item [%s] in [%hs]"),
			*GetNameSafe(InForItem), __FUNCTION__);
		return FObsidianDynamicItemAffix();
	}

	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(World);
	if (GameInstance == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Could not get GameInstance in [%hs]"), __FUNCTION__);
		return FObsidianDynamicItemAffix();
	}
	
	if (UObsidianItemDataLoaderSubsystem* ItemDataLoader = GameInstance->GetSubsystem<UObsidianItemDataLoaderSubsystem>())
	{
		TArray<FObsidianDynamicItemAffix> Suffixes;
		ItemDataLoader->GetAllSuffixesUpToQualityForCategory(InForItem->GetItemLevel(), InForItem->GetItemCategoryTag(),
			InForItem->GetItemBaseTypeTag(), /** OUT */ Suffixes);

		return GetRandomDynamicAffix(Suffixes);
	}

	UE_LOG(ObLogItems, Error, TEXT("ItemDataLoader is invalid in [%hs]"), __FUNCTION__);
	return FObsidianDynamicItemAffix();
}

FObsidianDynamicItemAffix UObsidianItemsFunctionLibrary::GetRandomDynamicAffix(const TArray<FObsidianDynamicItemAffix>& InDynamicAffixes)
{
	if (InDynamicAffixes.IsEmpty())
	{
		return FObsidianDynamicItemAffix();
	}

	uint32 TotalWeight = 0;
	for (const FObsidianDynamicItemAffix& Affix : InDynamicAffixes)
	{
		TotalWeight += Affix.AffixWeight;
	}

	const uint32 Roll = FMath::RandRange(0, TotalWeight);
	uint32 Cumulative = 0;
	for (const FObsidianDynamicItemAffix& Affix : InDynamicAffixes)
	{
		Cumulative += Affix.AffixWeight;
		if (Roll <= Cumulative)
		{
			return Affix;
		}
	}

	checkf(false, TEXT("No Affix was returned from GetRandomDynamicAffix."))
	return FObsidianDynamicItemAffix();
}

bool UObsidianItemsFunctionLibrary::FillItemGeneratedData(FObsidianItemGeneratedData& OutGeneratedData, const UObsidianInventoryItemInstance* InFromInstance)
{
	if (InFromInstance)
	{
		//NOTE(intrx) Do not initialize OutGeneratedData.AvailableStackCount here!! Stacks are handled in Stash/Inventory Components.
		OutGeneratedData.ItemLevel = InFromInstance->GetItemLevel();
		OutGeneratedData.ItemAffixes = InFromInstance->GetAllItemAffixes();
		OutGeneratedData.ItemRarity = InFromInstance->GetItemRarity();
		OutGeneratedData.NameData = FObsidianItemGeneratedNameData(InFromInstance->GetRareItemDisplayNameAddition(),
			InFromInstance->GetMagicAffixMultiplierItemDisplayNameAddition());
		OutGeneratedData.ItemEquippingRequirements = InFromInstance->GetEquippingRequirements();
		return true;
	}
	return false;
}

void UObsidianItemsFunctionLibrary::InitializeItemInstanceWithGeneratedData(UObsidianInventoryItemInstance* InInstance,
	const FObsidianItemGeneratedData& InGeneratedData)
{
	if (InInstance)
	{
		//NOTE(intrx) Do not initialize GeneratedData.AvailableStackCount here!! Stacks are handled in Stash/Inventory Components.
		InInstance->SetItemLevel(InGeneratedData.ItemLevel);
		InInstance->InitializeAffixes(InGeneratedData.ItemAffixes);
		InInstance->SetItemRarity(InGeneratedData.ItemRarity);
		InInstance->SetGeneratedNameAdditions(InGeneratedData.NameData);
		InInstance->InitializeEquippingRequirements(InGeneratedData.ItemEquippingRequirements);
	}
}

UObsidianInventoryItemInstance* UObsidianItemsFunctionLibrary::CreateItemInstanceFromDefinition(UObject* InOuter,
	const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDefClass, const FObsidianItemGeneratedData& InItemGeneratedData,
	const FObsidianItemPosition& InAtPosition)
{
	check(InOuter);
	check(InItemDefClass != nullptr);

	UObsidianInventoryItemInstance* Instance = NewObject<UObsidianInventoryItemInstance>(InOuter);
	Instance->SetItemDef(InItemDefClass);
	Instance->GenerateUniqueItemID();

	const UObsidianInventoryItemDefinition* DefaultObject = GetDefault<UObsidianInventoryItemDefinition>(InItemDefClass);
	for(const UObsidianInventoryItemFragment* Fragment : DefaultObject->ItemFragments)
	{
		if(Fragment)
		{
			Fragment->OnInstancedCreated(Instance);
		}
	}

	Instance->SetItemCurrentPosition(InAtPosition);
	Instance->SetItemCategory(DefaultObject->GetItemCategoryTag());
	Instance->SetItemBaseType(DefaultObject->GetItemBaseTypeTag());
	Instance->SetItemDebugName(DefaultObject->GetDebugName());
	InitializeItemInstanceWithGeneratedData(Instance, InItemGeneratedData);
	Instance->OnInstanceCreatedAndInitialized();

	return Instance;
}

int32 UObsidianItemsFunctionLibrary::GetAmountOfStacksAllowedToAddToItem(const AActor* InOwner, const UObsidianInventoryItemInstance* InAddingFromInstance, const UObsidianInventoryItemInstance* InInstanceToAddTo)
{
	if(InOwner == nullptr)
	{
		UE_LOG(ObLogInventory, Error, TEXT("Owner is nullptr in [%hs]"), __FUNCTION__);
		return 0; 
	}
	
	const int32 CurrentStackCount = InInstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	if(CurrentStackCount == 0)
	{
		return 0;
	}

	int32 CombinedStacks = 0;
	const int32 LimitStackCount = InInstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Limit);
	if(LimitStackCount > 0)
	{
		int32 StacksInInventory = 0;
		int32 StacksInStash = 0;
		if(UObsidianInventoryComponent* InventoryComp = UObsidianInventoryComponent::FindInventoryComponent(InOwner))
		{
			StacksInInventory = InventoryComp->FindAllStacksForGivenItem(InAddingFromInstance);
		}
		if(UObsidianPlayerStashComponent* PlayerStashComp = UObsidianPlayerStashComponent::FindPlayerStashComponent(InOwner))
		{
			StacksInStash = PlayerStashComp->FindAllStacksForGivenItem(InAddingFromInstance);
		}

		CombinedStacks = StacksInInventory + StacksInStash;
		ensureMsgf(CombinedStacks <= LimitStackCount, TEXT("Combined Stacks of held item is already bigger than Stacks Limit for this item, something went wrong."));
		if((LimitStackCount == 1) || (CombinedStacks >= LimitStackCount))
		{
			return 0;
		}
	}
	
	const int32 AddingFromInstanceCurrentStacks = InAddingFromInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	const int32 StacksThatCanBeAddedToInventory = LimitStackCount == 0 ? AddingFromInstanceCurrentStacks : LimitStackCount - CombinedStacks;
	if(StacksThatCanBeAddedToInventory <= 0)
	{
		return 0;
	}
			
	const int32 MaxStackCount = InInstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Max);
	const int32 AmountThatCanBeAddedToInstance = FMath::Clamp<int32>((MaxStackCount - CurrentStackCount), 0, StacksThatCanBeAddedToInventory);
	return FMath::Min<int32>(AmountThatCanBeAddedToInstance, AddingFromInstanceCurrentStacks);
}

int32 UObsidianItemsFunctionLibrary::GetAmountOfStacksAllowedToAddToItem_WithDef(const AActor* InOwner, const TSubclassOf<UObsidianInventoryItemDefinition>& InAddingFromItemDef, const int32 InAddingFromItemDefCurrentStacks, const UObsidianInventoryItemInstance* InInstanceToAddTo)
{
	const int32 CurrentStackCount = InInstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	if(CurrentStackCount == 0)
	{
		return 0;
	}

	int32 CombinedStacks = 0;
	const int32 LimitStackCount = InInstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Limit);
	if(LimitStackCount > 0)
	{
		int32 StacksInInventory = 0;
		int32 StacksInStash = 0;
		if(UObsidianInventoryComponent* InventoryComp = UObsidianInventoryComponent::FindInventoryComponent(InOwner))
		{
			StacksInInventory = InventoryComp->FindAllStacksForGivenItem(InAddingFromItemDef);
		}
		if(UObsidianPlayerStashComponent* PlayerStashComp = UObsidianPlayerStashComponent::FindPlayerStashComponent(InOwner))
		{
			StacksInStash = PlayerStashComp->FindAllStacksForGivenItem(InAddingFromItemDef);
		}

		CombinedStacks = StacksInInventory + StacksInStash;
		ensureMsgf(CombinedStacks <= LimitStackCount, TEXT("Combined Stacks of held item is already bigger than Stacks Limit for this item, something went wrong."));
		if((LimitStackCount == 1) || (CombinedStacks >= LimitStackCount))
		{
			return 0;
		}
	}
			
	const int32 StacksThatCanBeAddedToInventory = LimitStackCount == 0 ? InAddingFromItemDefCurrentStacks : LimitStackCount - CombinedStacks;
	if(StacksThatCanBeAddedToInventory <= 0)
	{
		return 0;
	}
			
	const int32 MaxStackCount = InInstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Max);
	const int32 AmountThatCanBeAddedToInstance = FMath::Clamp((MaxStackCount - CurrentStackCount), 0, StacksThatCanBeAddedToInventory);
	return FMath::Min(AmountThatCanBeAddedToInstance, InAddingFromItemDefCurrentStacks);
}

bool UObsidianItemsFunctionLibrary::IsItemUnique(const UObsidianInventoryItemInstance* InItemInstance)
{
	return InItemInstance->GetItemRarity() == EObsidianItemRarity::Unique;
}

bool UObsidianItemsFunctionLibrary::IsTwoHanded(const UObsidianInventoryItemInstance* InItemInstance)
{
	if (InItemInstance)
	{
		return UObsidianGameplayStatics::DoesTagMatchesAnySubTag(InItemInstance->GetItemCategoryTag(),
			TAG_Obsidian_TwoHand);
	}
	return false;
}

bool UObsidianItemsFunctionLibrary::IsTwoHanded_WithCategory(const FGameplayTag& InCategoryTag)
{
	if (InCategoryTag.IsValid())
	{
		return UObsidianGameplayStatics::DoesTagMatchesAnySubTag(InCategoryTag, TAG_Obsidian_TwoHand);
	}
	return false;
}

FGameplayTag UObsidianItemsFunctionLibrary::GetCategoryTagFromDraggedItem(const FDraggedItem& InDraggedItem)
{
	if (InDraggedItem.IsEmpty())
	{
		return FGameplayTag::EmptyTag;
	}

	if (const UObsidianInventoryItemInstance* DraggedInstance = InDraggedItem.Instance)
	{
		return DraggedInstance->GetItemCategoryTag();
	}

	if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = InDraggedItem.ItemDef)
	{
		if (const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(DraggedItemDef))
		{
			return ItemDefault->GetItemCategoryTag();
		}
	}
	
	return FGameplayTag::EmptyTag;
}

FGameplayTag UObsidianItemsFunctionLibrary::GetBaseTypeTagFromDraggedItem(const FDraggedItem& InDraggedItem)
{
	if (InDraggedItem.IsEmpty())
	{
		return FGameplayTag::EmptyTag;
	}

	if (const UObsidianInventoryItemInstance* DraggedInstance = InDraggedItem.Instance)
	{
		return DraggedInstance->GetItemBaseTypeTag();
	}

	if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = InDraggedItem.ItemDef)
	{
		if (const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(DraggedItemDef))
		{
			return ItemDefault->GetItemBaseTypeTag();
		}
	}
	
	return FGameplayTag::EmptyTag;
}

bool UObsidianItemsFunctionLibrary::GetItemCategoryAndBaseItemTypeTagsFromDraggedItem(const FDraggedItem& InDraggedItem,
	FGameplayTag& OutCategoryTag, FGameplayTag& OutItemBaseTypeTag)
{
	if (InDraggedItem.IsEmpty())
	{
		return false;
	}

	if (const UObsidianInventoryItemInstance* DraggedInstance = InDraggedItem.Instance)
	{
		OutCategoryTag = DraggedInstance->GetItemCategoryTag();
		OutItemBaseTypeTag = DraggedInstance->GetItemBaseTypeTag();
		return true;
	}

	if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = InDraggedItem.ItemDef)
	{
		if (const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(DraggedItemDef))
		{
			OutCategoryTag = ItemDefault->GetItemCategoryTag();
			OutItemBaseTypeTag = ItemDefault->GetItemBaseTypeTag();
			return true;
		}
	}
	return false;
}

FIntPoint UObsidianItemsFunctionLibrary::GetGridSpanFromDraggedItem(const FDraggedItem& InDraggedItem)
{
	if (InDraggedItem.IsEmpty())
	{
		return FIntPoint::NoneValue;
	}
	
	if(const UObsidianInventoryItemInstance* Instance = InDraggedItem.Instance)
	{
		return Instance->GetItemGridSpan();
	}
	
	if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = InDraggedItem.ItemDef)
	{
		if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
		{
			if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(
				ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
			{
				return AppearanceFrag->GetItemGridSpanFromDesc();
			}
		}
	}
	return FIntPoint::NoneValue;
}

bool UObsidianItemsFunctionLibrary::IsDefinitionIdentified(const UObsidianInventoryItemDefinition* InItemDefault, const FObsidianItemGeneratedData& InItemGeneratedData)
{
	if (InItemDefault)
	{
		/** Add any other conditions form ItemGeneratedData (Corrupted?). */
		return InItemDefault->DoesStartIdentified() || (InItemGeneratedData.ItemRarity <= EObsidianItemRarity::Normal);
	}
	return false;
}


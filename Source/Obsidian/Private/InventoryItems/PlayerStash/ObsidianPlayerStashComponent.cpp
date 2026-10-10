// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"

#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "InventoryItems/PlayerStash/ObsidianStashTab.h"
#include "InventoryItems/PlayerStash/ObsidianStashTabsConfig.h"
#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Slots.h"
#include "Obsidian/ObsidianLogCategories.h"


UObsidianPlayerStashComponent::UObsidianPlayerStashComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
	, StashItemList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
	
}

void UObsidianPlayerStashComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StashItemList);
	DOREPLIFETIME(ThisClass, CurrentStashTab);
}

TConstArrayView<TObjectPtr<UObsidianStashTab>> UObsidianPlayerStashComponent::GetAllStashTabs() const
{
	return StashTabs;
}

bool UObsidianPlayerStashComponent::CanOwnerModifyPlayerStashState()
{
	return CanOwnerModifyContainerState();
}

TArray<UObsidianInventoryItemInstance*> UObsidianPlayerStashComponent::GetAllItems() const
{
	return StashItemList.GetAllItems();
}

TArray<UObsidianInventoryItemInstance*> UObsidianPlayerStashComponent::GetAllPersonalItems() const
{
	return StashItemList.GetAllPersonalItems();	
}

TArray<UObsidianInventoryItemInstance*> UObsidianPlayerStashComponent::GetAllSharedItems() const
{
	return StashItemList.GetAllSharedItems();
}

TArray<UObsidianInventoryItemInstance*> UObsidianPlayerStashComponent::GetAllItemsFromStashTab(const FGameplayTag& InStashTabTag)
{
	return StashItemList.GetAllItemsFromStashTab(InStashTabTag);
}

UObsidianInventoryItemInstance* UObsidianPlayerStashComponent::GetItemInstanceFromTabAtPosition(const FObsidianItemPosition& InItemPosition)
{
	if (UObsidianStashTab* StashTab = GetStashTabForTag(InItemPosition.GetOwningStashTabTag()))
	{
		return StashTab->GetInstanceAtPosition(InItemPosition);
	}
	return nullptr;
}

void UObsidianPlayerStashComponent::InitializeStashTabs()
{
	if (ensure(StashTabsConfig))
	{
		StashTabs.Empty(StashTabsConfig->StashTabCount());
		StashTabs = StashItemList.InitializeStashTabs(StashTabsConfig);
	}
}

TArray<FObsidianStashSlotDefinition> UObsidianPlayerStashComponent::FindMatchingSlotsForItemCategory(const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseType)
{
	if (const UObsidianStashTab_Slots* SlotsStashTab = Cast<UObsidianStashTab_Slots>(GetStashTabForTag(GetActiveStashTag())))
	{
		return StashItemList.FindMatchingSlotsForItemCategory(InItemCategory, InItemBaseType, SlotsStashTab);
	}

	return {};
}

TArray<FObsidianStashSlotDefinition> UObsidianPlayerStashComponent::FindPossibleSlotsForPlacingItem_WithInstance(const UObsidianInventoryItemInstance* InForInstance)
{
	TArray<FObsidianStashSlotDefinition> MatchingSlots;
	if (InForInstance == nullptr)
	{
		return MatchingSlots;
	}
	
	if (UObsidianStashTab_Slots* SlotsStashTab = Cast<UObsidianStashTab_Slots>(GetStashTabForTag(GetActiveStashTag())))
	{
		const FGameplayTag ItemCategoryTag = InForInstance->GetItemCategoryTag();
		const FGameplayTag ItemBaseTypeTag = InForInstance->GetItemBaseTypeTag();
		for (const FObsidianStashSlotDefinition& PossibleSlot : StashItemList.FindMatchingSlotsForItemCategory(ItemCategoryTag, ItemBaseTypeTag, SlotsStashTab))
		{
			if (SlotsStashTab->CanPlaceItemAtSpecificPosition(PossibleSlot.GetStashSlotTag(), ItemCategoryTag, ItemBaseTypeTag, FIntPoint::NoneValue))
			{
				MatchingSlots.Add(PossibleSlot);
			}
		}
	}

	return MatchingSlots;
}

TArray<FObsidianStashSlotDefinition> UObsidianPlayerStashComponent::FindPossibleSlotsForPlacingItem_WithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& InForItemDef)
{
	TArray<FObsidianStashSlotDefinition> MatchingSlots;
	if (InForItemDef == nullptr)
	{
		return MatchingSlots;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = InForItemDef.GetDefaultObject();
	if (DefaultObject == nullptr)
	{
		return MatchingSlots;
	}
	
	if (UObsidianStashTab_Slots* SlotsStashTab = Cast<UObsidianStashTab_Slots>(GetStashTabForTag(GetActiveStashTag())))
	{
		const FGameplayTag ItemCategoryTag = DefaultObject->GetItemCategoryTag();
		const FGameplayTag ItemBaseTypeTag = DefaultObject->GetItemBaseTypeTag();
		for (const FObsidianStashSlotDefinition& PossibleSlot : StashItemList.FindMatchingSlotsForItemCategory(ItemCategoryTag, ItemBaseTypeTag, SlotsStashTab))
		{
			if (SlotsStashTab->CanPlaceItemAtSpecificPosition(PossibleSlot.GetStashSlotTag(), ItemCategoryTag, ItemBaseTypeTag, FIntPoint::NoneValue))
			{
				MatchingSlots.Add(PossibleSlot);
			}
		}
	}

	return MatchingSlots;
}

UObsidianStashTab* UObsidianPlayerStashComponent::GetStashTabForTag(const FGameplayTag& InStashTabTag)
{
	return StashItemList.GetStashTabForTag(InStashTabTag);
}

int32 UObsidianPlayerStashComponent::FindAllStacksForGivenItem(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef)
{
	//TODO(intrxx) Implement
	return 0;
}

int32 UObsidianPlayerStashComponent::FindAllStacksForGivenItem(const UObsidianInventoryItemInstance* InItemInstance)
{
	//TODO(intrxx) Implement
	return 0;
}

FObsidianAddingStacksResult UObsidianPlayerStashComponent::TryAddingStacksToExistingItems(const TSubclassOf<UObsidianInventoryItemDefinition>& InAddingFromItemDef, const int32 InStacksToAdd, const FGameplayTag& InTabTag, TArray<UObsidianInventoryItemInstance*>& OutAddedToInstances)
{
	FObsidianAddingStacksResult Result = FObsidianAddingStacksResult();
	Result.AddedStacks = 0;
	Result.StacksLeft = InStacksToAdd;
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if (CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}
	
	if (InStacksToAdd <= 0)
	{
		return Result;
	}
	
	TArray<UObsidianInventoryItemInstance*> Items = StashItemList.GetAllItems();
	for (UObsidianInventoryItemInstance* Instance : Items)
	{
		if (!IsValid(Instance))
		{
			UE_LOG(ObLogPlayerStash, Error, TEXT("Instance is invalid in [%hs]"), __FUNCTION__);
			continue;
		}
		
		if (InAddingFromItemDef == Instance->GetItemDef())
		{
			const int32 CurrentStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
			if (CurrentStackCount == 0)
			{
				continue;
			}
			
			const int32 StacksLeft = Result.StacksLeft;
			const int32 MaxStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Max);
			int32 AmountThatCanBeAddedToInstance = FMath::Clamp((MaxStackCount - CurrentStackCount), 0, StacksLeft);
			AmountThatCanBeAddedToInstance = FMath::Min(AmountThatCanBeAddedToInstance, StacksLeft);
			if (AmountThatCanBeAddedToInstance <= 0)
			{
				continue;
			}
			
			UE_LOG(ObLogPlayerStash, Verbose, TEXT("Added [%d] stacks to [%s]."), AmountThatCanBeAddedToInstance, *GetNameSafe(Instance));
			
			Instance->AddItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, AmountThatCanBeAddedToInstance);
			StashItemList.ChangedEntryStacks(Instance, CurrentStackCount, InTabTag);
			
			Result.AddedStacks += AmountThatCanBeAddedToInstance;
			Result.StacksLeft -= AmountThatCanBeAddedToInstance;
			OutAddedToInstances.AddUnique(Instance);
			
			if (Result.AddedStacks == InStacksToAdd)
			{
				Result.AddingStacksResult = EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded;
				return Result;
			}
		}
	}
	if (OutAddedToInstances.Num() > 0)
	{
		Result.AddingStacksResult = EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded;
	}
	return Result;
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> InItemDef,
	const FGameplayTag& InStashTabTag, const FObsidianItemGeneratedData& InItemGeneratedData)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	Result.StacksLeft = InItemGeneratedData.GetStackCount();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}
	
	if(CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}
	
	if(InItemDef == nullptr)
	{
		return Result;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = InItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return Result;
	}
	
	TArray<UObsidianInventoryItemInstance*> OutAddedToInstances;
	if(DefaultObject->IsStackable())
	{
		const FObsidianAddingStacksResult AddingStacksResult = TryAddingStacksToExistingItems(InItemDef, Result.StacksLeft, InStashTabTag, /** OUT */ OutAddedToInstances);

		if(AddingStacksResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded)
		{
			Result.StacksLeft = AddingStacksResult.StacksLeft;
			Result.AffectedInstance = OutAddedToInstances.Last();
		}
		
		if(AddingStacksResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded)
		{
			Result.bActionSuccessful = true;
			Result.StacksLeft = AddingStacksResult.StacksLeft;
			Result.AffectedInstance = OutAddedToInstances.Last();
			return Result;
		}
	}
	
	FObsidianItemPosition AvailablePosition;
	if(CanFitItemDefinition(AvailablePosition, InStashTabTag, InItemDef) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		UE_LOG(ObLogPlayerStash, Verbose, TEXT("Inventory is full!"));
		if(!OutAddedToInstances.IsEmpty())
		{
			Result.bActionSuccessful = true;
			Result.AffectedInstance = OutAddedToInstances.Last();
			return Result;
		}
		return Result;
	}
	
	UObsidianInventoryItemInstance* Instance = PlaceItemDefinition(InItemDef, InItemGeneratedData, Result.StacksLeft, AvailablePosition);

	Result.bActionSuccessful = true;
	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::AddItemDefinitionToSpecifiedSlot(const TSubclassOf<UObsidianInventoryItemDefinition> InItemDef, const FObsidianItemPosition& InItemPosition, const FObsidianItemGeneratedData& InItemGeneratedData, const int32 InStackToAddOverride)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	Result.StacksLeft = InItemGeneratedData.GetStackCount();

	ensure((InItemPosition.GetItemGridPosition(false) != FIntPoint::NoneValue || InItemPosition.GetItemSlotTag(false) != FGameplayTag::EmptyTag) && InItemPosition.GetOwningStashTabTag() != FGameplayTag::EmptyTag);
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}
	
	if(InItemDef == nullptr)
	{
		return Result;
	}
	
	const UObsidianInventoryItemDefinition* DefaultObject = InItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return Result;
	}

	int32 StacksAvailableToAdd = Result.StacksLeft;
	if(DefaultObject->IsStackable() && InStackToAddOverride != INDEX_NONE)
	{
		StacksAvailableToAdd = ClampStacksToAdd(StacksAvailableToAdd, InStackToAddOverride);
	}
	
	if(CanFitItemDefinitionToSpecifiedSlot(InItemPosition, InItemDef) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		UE_LOG(ObLogPlayerStash, Verbose, TEXT("Inventory is full at specified slot!"));
		return Result;
	}
	
	Result.StacksLeft -= StacksAvailableToAdd;
	Result.bActionSuccessful = Result.StacksLeft == 0; // We don't need any item duplication logic (like in AddItemInstanceToSpecificSlot)
														// as we still have a valid item definition in hands if the whole item isn't added here.
	ensure(Result.StacksLeft >= 0);
	
	UObsidianInventoryItemInstance* Instance = PlaceItemDefinition(InItemDef, InItemGeneratedData, StacksAvailableToAdd, InItemPosition);

	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianAddingStacksResult UObsidianPlayerStashComponent::TryAddingStacksToSpecificSlotWithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& InAddingFromItemDef, const int32 InAddingFromItemDefCurrentStacks, const FObsidianItemPosition& InAtPosition, const int32 InStackToAddOverride)
{
	return AddStacksToItemFromDefinition(InAddingFromItemDef, InAddingFromItemDefCurrentStacks,
		GetItemInstanceFromTabAtPosition(InAtPosition), InStackToAddOverride);
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::AddItemInstance(UObsidianInventoryItemInstance* InInstanceToAdd, const FGameplayTag& InStashTabTag)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}
	
	if(InInstanceToAdd == nullptr)
	{
		return Result;
	}

	Result.StacksLeft = InInstanceToAdd->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	
	if(CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}
	
	if(InInstanceToAdd->IsStackable())
	{
		TArray<UObsidianInventoryItemInstance*> OutAddedToInstances;
		const FObsidianAddingStacksResult AddingStacksResult = TryAddingStacksToExistingItems(InInstanceToAdd->GetItemDef(), Result.StacksLeft, InStashTabTag, /** OUT */ OutAddedToInstances);

		if(AddingStacksResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded)
		{
			Result.StacksLeft = AddingStacksResult.StacksLeft;
			Result.AffectedInstance = OutAddedToInstances.Last();
			InInstanceToAdd->OverrideItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, Result.StacksLeft);
		}
		
		if(AddingStacksResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded)
		{
			Result.bActionSuccessful = true;
			Result.StacksLeft = AddingStacksResult.StacksLeft;
			Result.AffectedInstance = OutAddedToInstances.Last();
			return Result;
		}
	}
	
	FObsidianItemPosition AvailablePosition;
	if(CheckAvailablePosition(AvailablePosition, InInstanceToAdd->GetItemGridSpan(), InInstanceToAdd->GetItemCategoryTag(), InInstanceToAdd->GetItemBaseTypeTag(), InStashTabTag) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		UE_LOG(ObLogPlayerStash, Verbose, TEXT("Inventory is full!"));
		return Result;
	}
	
	PlaceWholeItemInstance(InInstanceToAdd, AvailablePosition);
	
	Result.bActionSuccessful = true;
	Result.AffectedInstance = InInstanceToAdd;
	return Result;
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::AddItemInstanceToSpecificSlot(UObsidianInventoryItemInstance* InInstanceToAdd, const FObsidianItemPosition& InItemPosition, const int32 InStackToAddOverride)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();

	ensure((InItemPosition.GetItemGridPosition(false) != FIntPoint::NoneValue || InItemPosition.GetItemSlotTag(false) != FGameplayTag::EmptyTag) && InItemPosition.GetOwningStashTabTag() != FGameplayTag::EmptyTag);
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}
	
	if(InInstanceToAdd == nullptr)
	{
		return Result;
	}

	Result.StacksLeft = InInstanceToAdd->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);

	if(CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}

	int32 StacksAvailableToAdd = Result.StacksLeft;
	if(InInstanceToAdd->IsStackable() && InStackToAddOverride != INDEX_NONE)
	{
		StacksAvailableToAdd = ClampStacksToAdd(StacksAvailableToAdd, InStackToAddOverride);
	}
	
	if(CheckSpecifiedPosition(InItemPosition, InInstanceToAdd->GetItemCategoryTag(), InInstanceToAdd->GetItemBaseTypeTag(), InInstanceToAdd->GetItemGridSpan()) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		UE_LOG(ObLogPlayerStash, Verbose, TEXT("Player Stash is full at specified slot!"));
		return Result;
	}

	Result.bActionSuccessful = true;
	Result.StacksLeft -= StacksAvailableToAdd;
	ensure(Result.StacksLeft >= 0);
	
	bool bWholeItemPlaced = false;
	Result.AffectedInstance = PlaceItemInstance(InInstanceToAdd, StacksAvailableToAdd, InItemPosition, /** OUT */ bWholeItemPlaced);
	Result.bActionSuccessful = bWholeItemPlaced; // If only some of the stacks were placed, the rest is still held by the provided Instance.
	return Result;
}

FObsidianAddingStacksResult UObsidianPlayerStashComponent::TryAddingStacksToSpecificSlotWithInstance(UObsidianInventoryItemInstance* InAddingFromInstance, const FObsidianItemPosition& InAtPosition, const int32 InStackToAddOverride)
{
	return AddStacksToItemFromInstance(InAddingFromInstance, GetItemInstanceFromTabAtPosition(InAtPosition), InStackToAddOverride);
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::TakeOutFromItemInstance(UObsidianInventoryItemInstance* InTakingFromInstance, const int32 InStacksToTake)
{
	return TakeOutStacksFromItem(InTakingFromInstance, InStacksToTake);
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::RemoveItemInstance(UObsidianInventoryItemInstance* InInstanceToRemove)
{
	return RemoveItemFromContainer(InInstanceToRemove);
}

void UObsidianPlayerStashComponent::ServerRegisterAndValidateCurrentStashTab_Implementation(const FGameplayTag& InStashTab)
{
	if (InStashTab == FGameplayTag::EmptyTag)
	{
		CurrentStashTab = FGameplayTag::EmptyTag;
	}
	else if (GetStashTabForTag(InStashTab)) // Stash exists and is valid
	{
		CurrentStashTab = InStashTab;
	}
	else //TODO(intrxx) Should I do more here?
	{
		ensureAlwaysMsgf(false, TEXT("There is no such StashTab found for provided Gameplay Tag for Current Player."));
	}
}

FGameplayTag UObsidianPlayerStashComponent::GetActiveStashTag() const
{
	return CurrentStashTab;
}

void UObsidianPlayerStashComponent::LoadStashedItem(const FObsidianSavedItem& InStashedSavedItem)
{
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return;
	}

	UObsidianInventoryItemInstance* LoadedInstance = StashItemList.LoadEntry(InStashedSavedItem);
	
	RegisterItemInstanceForReplication(LoadedInstance);
}

TArray<UObsidianInventoryItemInstance*> UObsidianPlayerStashComponent::GetContainedItems() const
{
	return StashItemList.GetAllItems();
}

FGameplayTag UObsidianPlayerStashComponent::GetBlockActionsTag() const
{
	return ObsidianGameplayTags::PlayerStash::BlockActions;
}

void UObsidianPlayerStashComponent::AddItemInstanceToList(UObsidianInventoryItemInstance* InInstance,
	const FObsidianItemPosition& InToPosition)
{
	StashItemList.AddEntry(InInstance, InToPosition);
}

UObsidianInventoryItemInstance* UObsidianPlayerStashComponent::AddItemDefinitionToList(
	const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const FObsidianItemGeneratedData& InItemGeneratedData,
	const int32 InStackCount, const FObsidianItemPosition& InToPosition)
{
	return StashItemList.AddEntry(InItemDef, InItemGeneratedData, InStackCount, InToPosition);
}

void UObsidianPlayerStashComponent::RemoveItemInstanceFromList(UObsidianInventoryItemInstance* InInstance)
{
	const FObsidianItemPosition ItemPosition = InInstance->GetItemCurrentPosition();
	check(ItemPosition.GetOwningStashTabTag() != FGameplayTag::EmptyTag);

	StashItemList.RemoveEntry(InInstance, ItemPosition.GetOwningStashTabTag());
}

void UObsidianPlayerStashComponent::HandleItemStacksChanged(UObsidianInventoryItemInstance* InInstance, const int32 InOldStackCount)
{
	StashItemList.ChangedEntryStacks(InInstance, InOldStackCount, InInstance->GetItemCurrentPosition().GetOwningStashTabTag());
}

void UObsidianPlayerStashComponent::HandleItemChanged(UObsidianInventoryItemInstance* InInstance)
{
	StashItemList.GeneralEntryChange(InInstance, InInstance->GetItemCurrentPosition().GetOwningStashTabTag());
}

bool UObsidianPlayerStashComponent::CanFitItemDefinition(FObsidianItemPosition& OutAvailablePosition, const FGameplayTag& InStashTabTag, const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef)
{
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(InItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckAvailablePosition(OutAvailablePosition, AppearanceFrag->GetItemGridSpanFromDesc(), ItemDefault->GetItemCategoryTag(), ItemDefault->GetItemBaseTypeTag(), InStashTabTag);
		}
	}
	return false;
}

bool UObsidianPlayerStashComponent::CanFitItemDefinitionToSpecifiedSlot(const FObsidianItemPosition& InSpecifiedSlot, const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef)
{
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(InItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckSpecifiedPosition(InSpecifiedSlot, ItemDefault->GetItemCategoryTag(), ItemDefault->GetItemBaseTypeTag(), AppearanceFrag->GetItemGridSpanFromDesc());
		}
	}
	return false;
}

bool UObsidianPlayerStashComponent::CheckAvailablePosition(FObsidianItemPosition& OutAvailablePosition, const FIntPoint& InItemGridSpan, const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseTypeTag, const FGameplayTag& InStashTabTag)
{
	if (UObsidianStashTab* StashTab = GetStashTabForTag(InStashTabTag))
	{
		FObsidianItemPosition ItemPosition;
		if (StashTab->FindFirstAvailablePositionForItem(ItemPosition, InItemCategory, InItemBaseTypeTag, InItemGridSpan))
		{
			OutAvailablePosition = ItemPosition;
			return true;
		}
	}
	return false;
}

bool UObsidianPlayerStashComponent::CanReplaceItemAtPosition(const FObsidianItemPosition& InAtItemPosition, const UObsidianInventoryItemInstance* InReplacingInstance)
{
	if (InReplacingInstance == nullptr)
	{
		return false;
	}

	if (CanOwnerModifyPlayerStashState() == false)
	{
		return false;
	}
	
	if (UObsidianStashTab* StashTab = GetStashTabForTag(InAtItemPosition.GetOwningStashTabTag()))
	{
		return StashTab->CanReplaceItemAtSpecificPosition(InAtItemPosition, InReplacingInstance);
	}
	return false;
}


bool UObsidianPlayerStashComponent::CanReplaceItemAtPosition(const FObsidianItemPosition& InAtItemPosition, const TSubclassOf<UObsidianInventoryItemDefinition>& InReplacingDef)
{
	if (InReplacingDef == nullptr)
	{
		return false;
	}

	if (CanOwnerModifyPlayerStashState() == false)
	{
		return false;
	}
	
	if (UObsidianStashTab* StashTab = GetStashTabForTag(InAtItemPosition.GetOwningStashTabTag()))
	{
		return StashTab->CanReplaceItemAtSpecificPosition(InAtItemPosition, InReplacingDef);
	}
	return false;
}

bool UObsidianPlayerStashComponent::CheckSpecifiedPosition(const FObsidianItemPosition& InSpecifiedPosition, const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseTypeTag, const FIntPoint& InItemGridSpan)
{
	if (UObsidianStashTab* StashTab = GetStashTabForTag(InSpecifiedPosition.GetOwningStashTabTag()))
	{
		return StashTab->CanPlaceItemAtSpecificPosition(InSpecifiedPosition, InItemCategory, InItemBaseTypeTag, InItemGridSpan);
	}
	return false;
}

bool UObsidianPlayerStashComponent::CanFitInstanceInStashTab(const FIntPoint& InItemGridSpan, const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseTypeTag, const FGameplayTag& InStashTabTag)
{
	FObsidianItemPosition OutPosition;
	return CheckAvailablePosition(OutPosition, InItemGridSpan, InItemCategory, InItemBaseTypeTag, InStashTabTag);
}





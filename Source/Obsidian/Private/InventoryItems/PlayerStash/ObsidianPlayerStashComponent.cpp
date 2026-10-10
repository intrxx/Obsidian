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


UObsidianPlayerStashComponent::UObsidianPlayerStashComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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

TArray<UObsidianInventoryItemInstance*> UObsidianPlayerStashComponent::GetAllItemsFromStashTab(const FGameplayTag& StashTabTag)
{
	return StashItemList.GetAllItemsFromStashTab(StashTabTag);
}

UObsidianInventoryItemInstance* UObsidianPlayerStashComponent::GetItemInstanceFromTabAtPosition(const FObsidianItemPosition& ItemPosition)
{
	if (UObsidianStashTab* StashTab = GetStashTabForTag(ItemPosition.GetOwningStashTabTag()))
	{
		return StashTab->GetInstanceAtPosition(ItemPosition);
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

TArray<FObsidianStashSlotDefinition> UObsidianPlayerStashComponent::FindMatchingSlotsForItemCategory(const FGameplayTag& ItemCategory, const FGameplayTag& ItemBaseType)
{
	if (const UObsidianStashTab_Slots* SlotsStashTab = Cast<UObsidianStashTab_Slots>(GetStashTabForTag(GetActiveStashTag())))
	{
		return StashItemList.FindMatchingSlotsForItemCategory(ItemCategory, ItemBaseType, SlotsStashTab);
	}

	return {};
}

TArray<FObsidianStashSlotDefinition> UObsidianPlayerStashComponent::FindPossibleSlotsForPlacingItem_WithInstance(const UObsidianInventoryItemInstance* ForInstance)
{
	TArray<FObsidianStashSlotDefinition> MatchingSlots;
	if (ForInstance == nullptr)
	{
		return MatchingSlots;
	}
	
	if (UObsidianStashTab_Slots* SlotsStashTab = Cast<UObsidianStashTab_Slots>(GetStashTabForTag(GetActiveStashTag())))
	{
		const FGameplayTag ItemCategoryTag = ForInstance->GetItemCategoryTag();
		const FGameplayTag ItemBaseTypeTag = ForInstance->GetItemBaseTypeTag();
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

TArray<FObsidianStashSlotDefinition> UObsidianPlayerStashComponent::FindPossibleSlotsForPlacingItem_WithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& ForItemDef)
{
	TArray<FObsidianStashSlotDefinition> MatchingSlots;
	if (ForItemDef == nullptr)
	{
		return MatchingSlots;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = ForItemDef.GetDefaultObject();
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

UObsidianStashTab* UObsidianPlayerStashComponent::GetStashTabForTag(const FGameplayTag& StashTabTag)
{
	return StashItemList.GetStashTabForTag(StashTabTag);
}

int32 UObsidianPlayerStashComponent::FindAllStacksForGivenItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	//TODO(intrxx) Implement
	return 0;
}

int32 UObsidianPlayerStashComponent::FindAllStacksForGivenItem(const UObsidianInventoryItemInstance* ItemInstance)
{
	//TODO(intrxx) Implement
	return 0;
}

FObsidianAddingStacksResult UObsidianPlayerStashComponent::TryAddingStacksToExistingItems(const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 StacksToAdd, const FGameplayTag& InTabTag, TArray<UObsidianInventoryItemInstance*>& OutAddedToInstances)
{
	FObsidianAddingStacksResult Result = FObsidianAddingStacksResult();
	Result.AddedStacks = 0;
	Result.StacksLeft = StacksToAdd;
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if (CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}
	
	if (StacksToAdd <= 0)
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
		
		if (AddingFromItemDef == Instance->GetItemDef())
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
			
			if (Result.AddedStacks == StacksToAdd)
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

FObsidianItemOperationResult UObsidianPlayerStashComponent::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef,
	const FGameplayTag& StashTabTag, const FObsidianItemGeneratedData& ItemGeneratedData)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	Result.StacksLeft = ItemGeneratedData.GetStackCount();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}
	
	if(CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}
	
	if(ItemDef == nullptr)
	{
		return Result;
	}

	const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return Result;
	}
	
	TArray<UObsidianInventoryItemInstance*> OutAddedToInstances;
	if(DefaultObject->IsStackable())
	{
		const FObsidianAddingStacksResult AddingStacksResult = TryAddingStacksToExistingItems(ItemDef, Result.StacksLeft, StashTabTag, /** OUT */ OutAddedToInstances);

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
	if(CanFitItemDefinition(AvailablePosition, StashTabTag, ItemDef) == false)
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
	
	UObsidianInventoryItemInstance* Instance = PlaceItemDefinition(ItemDef, ItemGeneratedData, Result.StacksLeft, AvailablePosition);

	Result.bActionSuccessful = true;
	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::AddItemDefinitionToSpecifiedSlot(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FObsidianItemPosition& ItemPosition, const FObsidianItemGeneratedData& ItemGeneratedData, const int32 StackToAddOverride)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	Result.StacksLeft = ItemGeneratedData.GetStackCount();

	ensure((ItemPosition.GetItemGridPosition(false) != FIntPoint::NoneValue || ItemPosition.GetItemSlotTag(false) != FGameplayTag::EmptyTag) && ItemPosition.GetOwningStashTabTag() != FGameplayTag::EmptyTag);
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}
	
	if(ItemDef == nullptr)
	{
		return Result;
	}
	
	const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
	if(DefaultObject == nullptr)
	{
		return Result;
	}

	int32 StacksAvailableToAdd = Result.StacksLeft;
	if(DefaultObject->IsStackable() && StackToAddOverride != INDEX_NONE)
	{
		StacksAvailableToAdd = ClampStacksToAdd(StacksAvailableToAdd, StackToAddOverride);
	}
	
	if(CanFitItemDefinitionToSpecifiedSlot(ItemPosition, ItemDef) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		UE_LOG(ObLogPlayerStash, Verbose, TEXT("Inventory is full at specified slot!"));
		return Result;
	}
	
	Result.StacksLeft -= StacksAvailableToAdd;
	Result.bActionSuccessful = Result.StacksLeft == 0; // We don't need any item duplication logic (like in AddItemInstanceToSpecificSlot)
														// as we still have a valid item definition in hands if the whole item isn't added here.
	ensure(Result.StacksLeft >= 0);
	
	UObsidianInventoryItemInstance* Instance = PlaceItemDefinition(ItemDef, ItemGeneratedData, StacksAvailableToAdd, ItemPosition);

	Result.AffectedInstance = Instance;
	return Result;
}

FObsidianAddingStacksResult UObsidianPlayerStashComponent::TryAddingStacksToSpecificSlotWithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 AddingFromItemDefCurrentStacks, const FObsidianItemPosition& AtPosition, const int32 StackToAddOverride)
{
	return AddStacksToItemFromDefinition(AddingFromItemDef, AddingFromItemDefCurrentStacks,
		GetItemInstanceFromTabAtPosition(AtPosition), StackToAddOverride);
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd, const FGameplayTag& StashTabTag)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}
	
	if(InstanceToAdd == nullptr)
	{
		return Result;
	}

	Result.StacksLeft = InstanceToAdd->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	
	if(CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}
	
	if(InstanceToAdd->IsStackable())
	{
		TArray<UObsidianInventoryItemInstance*> OutAddedToInstances;
		const FObsidianAddingStacksResult AddingStacksResult = TryAddingStacksToExistingItems(InstanceToAdd->GetItemDef(), Result.StacksLeft, StashTabTag, /** OUT */ OutAddedToInstances);

		if(AddingStacksResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded)
		{
			Result.StacksLeft = AddingStacksResult.StacksLeft;
			Result.AffectedInstance = OutAddedToInstances.Last();
			InstanceToAdd->OverrideItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, Result.StacksLeft);
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
	if(CheckAvailablePosition(AvailablePosition, InstanceToAdd->GetItemGridSpan(), InstanceToAdd->GetItemCategoryTag(), InstanceToAdd->GetItemBaseTypeTag(), StashTabTag) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		UE_LOG(ObLogPlayerStash, Verbose, TEXT("Inventory is full!"));
		return Result;
	}
	
	PlaceWholeItemInstance(InstanceToAdd, AvailablePosition);
	
	Result.bActionSuccessful = true;
	Result.AffectedInstance = InstanceToAdd;
	return Result;
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::AddItemInstanceToSpecificSlot(UObsidianInventoryItemInstance* InstanceToAdd, const FObsidianItemPosition& ItemPosition, const int32 StackToAddOverride)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();

	ensure((ItemPosition.GetItemGridPosition(false) != FIntPoint::NoneValue || ItemPosition.GetItemSlotTag(false) != FGameplayTag::EmptyTag) && ItemPosition.GetOwningStashTabTag() != FGameplayTag::EmptyTag);
	
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}
	
	if(InstanceToAdd == nullptr)
	{
		return Result;
	}

	Result.StacksLeft = InstanceToAdd->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);

	if(CanOwnerModifyPlayerStashState() == false)
	{
		return Result;
	}

	int32 StacksAvailableToAdd = Result.StacksLeft;
	if(InstanceToAdd->IsStackable() && StackToAddOverride != INDEX_NONE)
	{
		StacksAvailableToAdd = ClampStacksToAdd(StacksAvailableToAdd, StackToAddOverride);
	}
	
	if(CheckSpecifiedPosition(ItemPosition, InstanceToAdd->GetItemCategoryTag(), InstanceToAdd->GetItemBaseTypeTag(), InstanceToAdd->GetItemGridSpan()) == false)
	{
		//TODO(intrxx) Inventory is full, add voice over?
		UE_LOG(ObLogPlayerStash, Verbose, TEXT("Player Stash is full at specified slot!"));
		return Result;
	}

	Result.bActionSuccessful = true;
	Result.StacksLeft -= StacksAvailableToAdd;
	ensure(Result.StacksLeft >= 0);
	
	bool bWholeItemPlaced = false;
	Result.AffectedInstance = PlaceItemInstance(InstanceToAdd, StacksAvailableToAdd, ItemPosition, /** OUT */ bWholeItemPlaced);
	Result.bActionSuccessful = bWholeItemPlaced; // If only some of the stacks were placed, the rest is still held by the provided Instance.
	return Result;
}

FObsidianAddingStacksResult UObsidianPlayerStashComponent::TryAddingStacksToSpecificSlotWithInstance(UObsidianInventoryItemInstance* AddingFromInstance, const FObsidianItemPosition& AtPosition, const int32 StackToAddOverride)
{
	return AddStacksToItemFromInstance(AddingFromInstance, GetItemInstanceFromTabAtPosition(AtPosition), StackToAddOverride);
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::TakeOutFromItemInstance(UObsidianInventoryItemInstance* TakingFromInstance, const int32 StacksToTake)
{
	return TakeOutStacksFromItem(TakingFromInstance, StacksToTake);
}

FObsidianItemOperationResult UObsidianPlayerStashComponent::RemoveItemInstance(UObsidianInventoryItemInstance* InstanceToRemove)
{
	return RemoveItemFromContainer(InstanceToRemove);
}

void UObsidianPlayerStashComponent::ServerRegisterAndValidateCurrentStashTab_Implementation(const FGameplayTag& StashTab)
{
	if (StashTab == FGameplayTag::EmptyTag)
	{
		CurrentStashTab = FGameplayTag::EmptyTag;
	}
	else if (GetStashTabForTag(StashTab)) // Stash exists and is valid
	{
		CurrentStashTab = StashTab;
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

void UObsidianPlayerStashComponent::LoadStashedItem(const FObsidianSavedItem& StashedSavedItem)
{
	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return;
	}

	UObsidianInventoryItemInstance* LoadedInstance = StashItemList.LoadEntry(StashedSavedItem);
	
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

void UObsidianPlayerStashComponent::AddItemInstanceToList(UObsidianInventoryItemInstance* Instance,
	const FObsidianItemPosition& ToPosition)
{
	StashItemList.AddEntry(Instance, ToPosition);
}

UObsidianInventoryItemInstance* UObsidianPlayerStashComponent::AddItemDefinitionToList(
	const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FObsidianItemGeneratedData& ItemGeneratedData,
	const int32 StackCount, const FObsidianItemPosition& ToPosition)
{
	return StashItemList.AddEntry(ItemDef, ItemGeneratedData, StackCount, ToPosition);
}

void UObsidianPlayerStashComponent::RemoveItemInstanceFromList(UObsidianInventoryItemInstance* Instance)
{
	const FObsidianItemPosition ItemPosition = Instance->GetItemCurrentPosition();
	check(ItemPosition.GetOwningStashTabTag() != FGameplayTag::EmptyTag);

	StashItemList.RemoveEntry(Instance, ItemPosition.GetOwningStashTabTag());
}

void UObsidianPlayerStashComponent::HandleItemStacksChanged(UObsidianInventoryItemInstance* Instance, const int32 OldStackCount)
{
	StashItemList.ChangedEntryStacks(Instance, OldStackCount, Instance->GetItemCurrentPosition().GetOwningStashTabTag());
}

void UObsidianPlayerStashComponent::HandleItemChanged(UObsidianInventoryItemInstance* Instance)
{
	StashItemList.GeneralEntryChange(Instance, Instance->GetItemCurrentPosition().GetOwningStashTabTag());
}

bool UObsidianPlayerStashComponent::CanFitItemDefinition(FObsidianItemPosition& OutAvailablePosition, const FGameplayTag& StashTabTag, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckAvailablePosition(OutAvailablePosition, AppearanceFrag->GetItemGridSpanFromDesc(), ItemDefault->GetItemCategoryTag(), ItemDefault->GetItemBaseTypeTag(), StashTabTag);
		}
	}
	return false;
}

bool UObsidianPlayerStashComponent::CanFitItemDefinitionToSpecifiedSlot(const FObsidianItemPosition& SpecifiedSlot, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			return CheckSpecifiedPosition(SpecifiedSlot, ItemDefault->GetItemCategoryTag(), ItemDefault->GetItemBaseTypeTag(), AppearanceFrag->GetItemGridSpanFromDesc());
		}
	}
	return false;
}

bool UObsidianPlayerStashComponent::CheckAvailablePosition(FObsidianItemPosition& OutAvailablePosition, const FIntPoint& ItemGridSpan, const FGameplayTag& ItemCategory, const FGameplayTag& ItemBaseTypeTag, const FGameplayTag& StashTabTag)
{
	if (UObsidianStashTab* StashTab = GetStashTabForTag(StashTabTag))
	{
		FObsidianItemPosition ItemPosition;
		if (StashTab->FindFirstAvailablePositionForItem(ItemPosition, ItemCategory, ItemBaseTypeTag, ItemGridSpan))
		{
			OutAvailablePosition = ItemPosition;
			return true;
		}
	}
	return false;
}

bool UObsidianPlayerStashComponent::CanReplaceItemAtPosition(const FObsidianItemPosition& AtItemPosition, const UObsidianInventoryItemInstance* ReplacingInstance)
{
	if (ReplacingInstance == nullptr)
	{
		return false;
	}

	if (CanOwnerModifyPlayerStashState() == false)
	{
		return false;
	}
	
	if (UObsidianStashTab* StashTab = GetStashTabForTag(AtItemPosition.GetOwningStashTabTag()))
	{
		return StashTab->CanReplaceItemAtSpecificPosition(AtItemPosition, ReplacingInstance);
	}
	return false;
}


bool UObsidianPlayerStashComponent::CanReplaceItemAtPosition(const FObsidianItemPosition& AtItemPosition, const TSubclassOf<UObsidianInventoryItemDefinition>& ReplacingDef)
{
	if (ReplacingDef == nullptr)
	{
		return false;
	}

	if (CanOwnerModifyPlayerStashState() == false)
	{
		return false;
	}
	
	if (UObsidianStashTab* StashTab = GetStashTabForTag(AtItemPosition.GetOwningStashTabTag()))
	{
		return StashTab->CanReplaceItemAtSpecificPosition(AtItemPosition, ReplacingDef);
	}
	return false;
}

bool UObsidianPlayerStashComponent::CheckSpecifiedPosition(const FObsidianItemPosition& SpecifiedPosition, const FGameplayTag& ItemCategory, const FGameplayTag& ItemBaseTypeTag, const FIntPoint& ItemGridSpan)
{
	if (UObsidianStashTab* StashTab = GetStashTabForTag(SpecifiedPosition.GetOwningStashTabTag()))
	{
		return StashTab->CanPlaceItemAtSpecificPosition(SpecifiedPosition, ItemCategory, ItemBaseTypeTag, ItemGridSpan);
	}
	return false;
}

bool UObsidianPlayerStashComponent::CanFitInstanceInStashTab(const FIntPoint& ItemGridSpan, const FGameplayTag& ItemCategory, const FGameplayTag& ItemBaseTypeTag, const FGameplayTag& StashTabTag)
{
	FObsidianItemPosition OutPosition;
	return CheckAvailablePosition(OutPosition, ItemGridSpan, ItemCategory, ItemBaseTypeTag, StashTabTag);
}





// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ObsidianItemContainerComponent.h"

#include "Engine/ActorChannel.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "Obsidian/ObsidianLogCategories.h"


UObsidianItemContainerComponent::UObsidianItemContainerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

AObsidianPlayerController* UObsidianItemContainerComponent::GetOwnerPlayerController() const
{
	return Cast<AObsidianPlayerController>(GetOwner());
}

AObsidianPlayerState* UObsidianItemContainerComponent::GetObsidianPlayerStateFromOwner() const
{
	if(const AObsidianPlayerController* OwningPlayerController = GetOwnerPlayerController())
	{
		return OwningPlayerController->GetObsidianPlayerState();
	}
	return nullptr;
}

UObsidianAbilitySystemComponent* UObsidianItemContainerComponent::GetObsidianAbilitySystemComponentFromOwner() const
{
	if(const AObsidianPlayerController* OwningPlayerController = GetOwnerPlayerController())
	{
		return OwningPlayerController->GetObsidianAbilitySystemComponent();
	}
	return nullptr;
}

UObsidianInventoryComponent* UObsidianItemContainerComponent::GetInventoryComponentFromOwner() const
{
	if(const AObsidianPlayerController* OwningPlayerController = GetOwnerPlayerController())
	{
		return OwningPlayerController->GetInventoryComponent();
	}
	return nullptr;
}

UObsidianEquipmentComponent* UObsidianItemContainerComponent::GetEquipmentComponentFromOwner() const
{
	if(const AObsidianPlayerController* OwningPlayerController = GetOwnerPlayerController())
	{
		return OwningPlayerController->GetEquipmentComponent();
	}
	return nullptr;
}

UObsidianPlayerStashComponent* UObsidianItemContainerComponent::GetStashComponentFromOwner() const
{
	if(const AObsidianPlayerController* OwningPlayerController = GetOwnerPlayerController())
	{
		return OwningPlayerController->GetPlayerStashComponent();
	}
	return nullptr;
}

UObsidianItemContainerComponent* UObsidianItemContainerComponent::GetContainerFromOwnerForPosition(
	const FObsidianItemPosition& ItemPosition) const
{
	if(ItemPosition.IsOnInventoryGrid())
	{
		return GetInventoryComponentFromOwner();
	}
	if(ItemPosition.IsOnStash())
	{
		return GetStashComponentFromOwner();
	}
	if(ItemPosition.IsOnEquipmentSlot())
	{
		return GetEquipmentComponentFromOwner();
	}
	return nullptr;
}

bool UObsidianItemContainerComponent::CanOwnerModifyContainerState() const
{
	if(const UObsidianAbilitySystemComponent* OwnerASC = GetObsidianAbilitySystemComponentFromOwner())
	{
		return !OwnerASC->HasMatchingGameplayTag(GetBlockActionsTag());
	}
	return false;
}

int32 UObsidianItemContainerComponent::CountStacksOfItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef) const
{
	int32 AllStacks = 0;
	for(const UObsidianInventoryItemInstance* Instance : GetContainedItems())
	{
		if(IsValid(Instance) && Instance->GetItemDef() == ItemDef)
		{
			AllStacks += Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
		}
	}
	return AllStacks;
}

void UObsidianItemContainerComponent::UseItem(UObsidianInventoryItemInstance* UsingInstance,
	UObsidianInventoryItemInstance* UsingOntoInstance)
{
	if(UsingInstance == nullptr)
	{
		UE_LOG(ObLogItemContainer, Error, TEXT("UsingInstance is invalid in [%hs]"), __FUNCTION__);
		return;
	}

	if(UsingInstance->IsItemUsable() == false)
	{
		UE_LOG(ObLogItemContainer, Error, TEXT("Trying to use unusable Item [%s] in [%hs]"),
			*UsingInstance->GetItemDebugName(), __FUNCTION__);
		return;
	}

	AObsidianPlayerController* OwningPlayerController = GetOwnerPlayerController();
	if(OwningPlayerController == nullptr || OwningPlayerController->HasAuthority() == false)
	{
		return;
	}

	const int32 CurrentUsingInstanceStacks = UsingInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	if(CurrentUsingInstanceStacks <= 0)
	{
		UE_LOG(ObLogItemContainer, Error, TEXT("Trying to use Item [%s] that has no more stacks in [%hs]"),
			*UsingInstance->GetItemDebugName(), __FUNCTION__);
		return;
	}

	bool bUsageSuccessful = false;
	const EObsidianUsableItemType ItemType = UsingInstance->GetUsableItemType();
	if(ItemType == EObsidianUsableItemType::UIT_Crafting)
	{
		if(UsingOntoInstance == nullptr)
		{
			UE_LOG(ObLogItemContainer, Error, TEXT("UsingOntoInstance is invalid in [%hs]"), __FUNCTION__);
			return;
		}

		if(UsingInstance->UseItem(OwningPlayerController, UsingOntoInstance))
		{
			// The item we used onto does not need to be held by this container.
			if(UObsidianItemContainerComponent* Container = GetContainerFromOwnerForPosition(UsingOntoInstance->GetItemCurrentPosition()))
			{
				Container->HandleItemChanged(UsingOntoInstance);
			}
			bUsageSuccessful = true;
		}
	}
	else if(ItemType == EObsidianUsableItemType::UIT_Activation)
	{
		bUsageSuccessful = UsingInstance->UseItem(OwningPlayerController, nullptr);
	}

	if(bUsageSuccessful)
	{
		UpdateUsingItemAfterUsage(UsingInstance, CurrentUsingInstanceStacks);
	}
	else
	{
		//TODO(intrxx) Usage failed, Play some VO?
	}
}

void UObsidianItemContainerComponent::UpdateUsingItemAfterUsage(UObsidianInventoryItemInstance* UsingInstance,
	const int32 CurrentStacks)
{
	if(UsingInstance == nullptr)
	{
		return;
	}

	if(UObsidianItemContainerComponent* Container = GetContainerFromOwnerForPosition(UsingInstance->GetItemCurrentPosition()))
	{
		Container->ConsumeUsedItem(UsingInstance, CurrentStacks);
	}
}

bool UObsidianItemContainerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for(UObsidianInventoryItemInstance* Instance : GetContainedItems())
	{
		if(IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UObsidianItemContainerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing Item Instances
	if(IsUsingRegisteredSubObjectList())
	{
		for(UObsidianInventoryItemInstance* Instance : GetContainedItems())
		{
			if(IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

bool UObsidianItemContainerComponent::HasOwnerAuthority(const ANSICHAR* CallingFunction) const
{
	const AActor* Owner = GetOwner();
	if(Owner && Owner->HasAuthority())
	{
		return true;
	}

	UE_LOG(ObLogItemContainer, Warning, TEXT("No Authority in [%hs]"), CallingFunction);
	return false;
}

void UObsidianItemContainerComponent::RegisterItemInstanceForReplication(UObsidianInventoryItemInstance* Instance)
{
	if(Instance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}
}

void UObsidianItemContainerComponent::UnregisterItemInstanceFromReplication(UObsidianInventoryItemInstance* Instance)
{
	if(Instance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(Instance);
	}
}

int32 UObsidianItemContainerComponent::ClampStacksToAdd(const int32 StacksAvailableToAdd, const int32 StackToAddOverride)
{
	if(StackToAddOverride == INDEX_NONE)
	{
		return StacksAvailableToAdd;
	}
	return FMath::Clamp<int32>(FMath::Min<int32>(StacksAvailableToAdd, StackToAddOverride), 1, StacksAvailableToAdd);
}

UObsidianInventoryItemInstance* UObsidianItemContainerComponent::PlaceItemDefinition(
	const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FObsidianItemGeneratedData& ItemGeneratedData,
	const int32 StackCount, const FObsidianItemPosition& ToPosition)
{
	UObsidianInventoryItemInstance* Instance = AddItemDefinitionToList(ItemDef, ItemGeneratedData, StackCount, ToPosition);
	if(Instance == nullptr)
	{
		return nullptr;
	}

	Instance->AddItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, StackCount);
	Instance->SetIdentified(UObsidianItemsFunctionLibrary::IsDefinitionIdentified(ItemDef.GetDefaultObject(), ItemGeneratedData));

	RegisterItemInstanceForReplication(Instance);
	return Instance;
}

void UObsidianItemContainerComponent::PlaceWholeItemInstance(UObsidianInventoryItemInstance* InstanceToAdd,
	const FObsidianItemPosition& ToPosition)
{
	AddItemInstanceToList(InstanceToAdd, ToPosition);
	RegisterItemInstanceForReplication(InstanceToAdd);
}

UObsidianInventoryItemInstance* UObsidianItemContainerComponent::PlaceItemInstance(UObsidianInventoryItemInstance* InstanceToAdd,
	const int32 StacksToAdd, const FObsidianItemPosition& ToPosition, bool& bOutWholeItemPlaced)
{
	const int32 CurrentHeldItemStacks = InstanceToAdd->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	bOutWholeItemPlaced = StacksToAdd == CurrentHeldItemStacks;
	if(bOutWholeItemPlaced)
	{
		PlaceWholeItemInstance(InstanceToAdd, ToPosition);
		return InstanceToAdd;
	}

	// Only some of the stacks are placed, the rest stays on the provided Instance and a new one is created for the placed part.
	InstanceToAdd->OverrideItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, CurrentHeldItemStacks - StacksToAdd);
	const bool bCachedIdentified = InstanceToAdd->IsItemIdentified();

	FObsidianItemGeneratedData CachedGeneratedData;
	UObsidianItemsFunctionLibrary::FillItemGeneratedData(CachedGeneratedData, InstanceToAdd);

	UObsidianInventoryItemInstance* SplitInstance = AddItemDefinitionToList(InstanceToAdd->GetItemDef(), CachedGeneratedData,
		StacksToAdd, ToPosition);
	if(SplitInstance)
	{
		SplitInstance->AddItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, StacksToAdd);
		SplitInstance->SetIdentified(bCachedIdentified);

		RegisterItemInstanceForReplication(SplitInstance);
	}
	return SplitInstance;
}

FObsidianAddingStacksResult UObsidianItemContainerComponent::AddStacksToItemFromInstance(
	UObsidianInventoryItemInstance* AddingFromInstance, UObsidianInventoryItemInstance* InstanceToAddTo,
	const int32 StackToAddOverride)
{
	FObsidianAddingStacksResult Result = FObsidianAddingStacksResult();

	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(AddingFromInstance == nullptr)
	{
		return Result;
	}

	const int32 AddingFromInstanceCurrentStacks = AddingFromInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	Result.StacksLeft = AddingFromInstanceCurrentStacks;

	if(CanOwnerModifyContainerState() == false)
	{
		return Result;
	}

	if(UObsidianItemsFunctionLibrary::IsTheSameItem(AddingFromInstance, InstanceToAddTo) == false)
	{
		return Result;
	}

	int32 AmountThatCanBeAddedToInstance = UObsidianItemsFunctionLibrary::GetAmountOfStacksAllowedToAddToItem(GetOwner(),
		AddingFromInstance, InstanceToAddTo);
	if(AmountThatCanBeAddedToInstance <= 0)
	{
		return Result;
	}

	AmountThatCanBeAddedToInstance = ClampStacksToAdd(AmountThatCanBeAddedToInstance, StackToAddOverride);

	const int32 OldStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	InstanceToAddTo->AddItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, AmountThatCanBeAddedToInstance);
	HandleItemStacksChanged(InstanceToAddTo, OldStackCount);

	// AddingFromInstance is not held by this container, whoever holds it is responsible for reacting to the change.
	AddingFromInstance->RemoveItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, AmountThatCanBeAddedToInstance);

	Result.AddedStacks = AmountThatCanBeAddedToInstance;
	Result.StacksLeft -= AmountThatCanBeAddedToInstance;
	Result.LastAddedToInstance = InstanceToAddTo;
	Result.AddingStacksResult = AmountThatCanBeAddedToInstance == AddingFromInstanceCurrentStacks
		? EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded
		: EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded;
	return Result;
}

FObsidianAddingStacksResult UObsidianItemContainerComponent::AddStacksToItemFromDefinition(
	const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 AddingFromItemDefCurrentStacks,
	UObsidianInventoryItemInstance* InstanceToAddTo, const int32 StackToAddOverride)
{
	FObsidianAddingStacksResult Result = FObsidianAddingStacksResult();
	Result.StacksLeft = AddingFromItemDefCurrentStacks;

	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(CanOwnerModifyContainerState() == false)
	{
		return Result;
	}

	if(UObsidianItemsFunctionLibrary::IsTheSameItem_WithDef(InstanceToAddTo, AddingFromItemDef) == false)
	{
		return Result;
	}

	int32 AmountThatCanBeAddedToInstance = UObsidianItemsFunctionLibrary::GetAmountOfStacksAllowedToAddToItem_WithDef(GetOwner(),
		AddingFromItemDef, AddingFromItemDefCurrentStacks, InstanceToAddTo);
	if(AmountThatCanBeAddedToInstance <= 0)
	{
		return Result;
	}

	AmountThatCanBeAddedToInstance = ClampStacksToAdd(AmountThatCanBeAddedToInstance, StackToAddOverride);

	const int32 OldStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	InstanceToAddTo->AddItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, AmountThatCanBeAddedToInstance);
	HandleItemStacksChanged(InstanceToAddTo, OldStackCount);

	Result.StacksLeft -= AmountThatCanBeAddedToInstance;
	Result.AddedStacks = AmountThatCanBeAddedToInstance;
	Result.LastAddedToInstance = InstanceToAddTo;
	Result.AddingStacksResult = AmountThatCanBeAddedToInstance == AddingFromItemDefCurrentStacks
		? EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded
		: EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded;
	return Result;
}

FObsidianItemOperationResult UObsidianItemContainerComponent::TakeOutStacksFromItem(
	UObsidianInventoryItemInstance* TakingFromInstance, const int32 StacksToTake)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();

	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(CanOwnerModifyContainerState() == false)
	{
		return Result;
	}

	if(TakingFromInstance == nullptr)
	{
		return Result;
	}

	// The only valid number of stacks to take is in range [1, x - 1], taking the whole item out is just picking it up.
	// StacksToTake comes from the Client so it can't be trusted, anything outside this range would duplicate or void stacks.
	const int32 CurrentTakingFromInstanceStacks = TakingFromInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	if(StacksToTake < 1 || StacksToTake >= CurrentTakingFromInstanceStacks)
	{
		return Result;
	}

	TakingFromInstance->RemoveItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, StacksToTake);
	HandleItemStacksChanged(TakingFromInstance, CurrentTakingFromInstanceStacks);

	Result.bActionSuccessful = true;
	Result.AffectedInstance = TakingFromInstance;
	Result.StacksLeft = StacksToTake;
	return Result;
}

FObsidianItemOperationResult UObsidianItemContainerComponent::RemoveItemFromContainer(UObsidianInventoryItemInstance* InstanceToRemove)
{
	FObsidianItemOperationResult Result = FObsidianItemOperationResult();

	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(CanOwnerModifyContainerState() == false)
	{
		return Result;
	}

	if(InstanceToRemove == nullptr)
	{
		UE_LOG(ObLogItemContainer, Error, TEXT("Passed InstanceToRemove is invalid in [%hs]"), __FUNCTION__);
		return Result;
	}

	RemoveItemInstanceFromList(InstanceToRemove);
	UnregisterItemInstanceFromReplication(InstanceToRemove);

	Result.bActionSuccessful = true;
	Result.AffectedInstance = InstanceToRemove;
	return Result;
}

void UObsidianItemContainerComponent::ConsumeUsedItem(UObsidianInventoryItemInstance* UsingInstance, const int32 CurrentStacks)
{
	if(UsingInstance == nullptr)
	{
		return;
	}

	if(CurrentStacks > 1)
	{
		UsingInstance->RemoveItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, 1);
		HandleItemStacksChanged(UsingInstance, CurrentStacks);
		return;
	}

	RemoveItemInstanceFromList(UsingInstance);
	UnregisterItemInstanceFromReplication(UsingInstance);
}

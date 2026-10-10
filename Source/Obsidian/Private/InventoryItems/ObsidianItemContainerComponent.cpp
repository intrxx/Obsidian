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


UObsidianItemContainerComponent::UObsidianItemContainerComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
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
	const FObsidianItemPosition& InItemPosition) const
{
	if(InItemPosition.IsOnInventoryGrid())
	{
		return GetInventoryComponentFromOwner();
	}
	if(InItemPosition.IsOnStash())
	{
		return GetStashComponentFromOwner();
	}
	if(InItemPosition.IsOnEquipmentSlot())
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

int32 UObsidianItemContainerComponent::CountStacksOfItem(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef) const
{
	int32 AllStacks = 0;
	for(const UObsidianInventoryItemInstance* Instance : GetContainedItems())
	{
		if(IsValid(Instance) && Instance->GetItemDef() == InItemDef)
		{
			AllStacks += Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
		}
	}
	return AllStacks;
}

void UObsidianItemContainerComponent::UseItem(UObsidianInventoryItemInstance* InUsingInstance,
	UObsidianInventoryItemInstance* InUsingOntoInstance)
{
	if(InUsingInstance == nullptr)
	{
		UE_LOG(ObLogItemContainer, Error, TEXT("UsingInstance is invalid in [%hs]"), __FUNCTION__);
		return;
	}

	if(InUsingInstance->IsItemUsable() == false)
	{
		UE_LOG(ObLogItemContainer, Error, TEXT("Trying to use unusable Item [%s] in [%hs]"),
			*InUsingInstance->GetItemDebugName(), __FUNCTION__);
		return;
	}

	AObsidianPlayerController* OwningPlayerController = GetOwnerPlayerController();
	if(OwningPlayerController == nullptr || OwningPlayerController->HasAuthority() == false)
	{
		return;
	}

	const int32 CurrentUsingInstanceStacks = InUsingInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	if(CurrentUsingInstanceStacks <= 0)
	{
		UE_LOG(ObLogItemContainer, Error, TEXT("Trying to use Item [%s] that has no more stacks in [%hs]"),
			*InUsingInstance->GetItemDebugName(), __FUNCTION__);
		return;
	}

	bool bUsageSuccessful = false;
	const EObsidianUsableItemType ItemType = InUsingInstance->GetUsableItemType();
	if(ItemType == EObsidianUsableItemType::UIT_Crafting)
	{
		if(InUsingOntoInstance == nullptr)
		{
			UE_LOG(ObLogItemContainer, Error, TEXT("UsingOntoInstance is invalid in [%hs]"), __FUNCTION__);
			return;
		}

		if(InUsingInstance->UseItem(OwningPlayerController, InUsingOntoInstance))
		{
			// The item we used onto does not need to be held by this container.
			if(UObsidianItemContainerComponent* Container = GetContainerFromOwnerForPosition(InUsingOntoInstance->GetItemCurrentPosition()))
			{
				Container->HandleItemChanged(InUsingOntoInstance);
			}
			bUsageSuccessful = true;
		}
	}
	else if(ItemType == EObsidianUsableItemType::UIT_Activation)
	{
		bUsageSuccessful = InUsingInstance->UseItem(OwningPlayerController, nullptr);
	}

	if(bUsageSuccessful)
	{
		UpdateUsingItemAfterUsage(InUsingInstance, CurrentUsingInstanceStacks);
	}
	else
	{
		//TODO(intrxx) Usage failed, Play some VO?
	}
}

void UObsidianItemContainerComponent::UpdateUsingItemAfterUsage(UObsidianInventoryItemInstance* InUsingInstance,
	const int32 InCurrentStacks)
{
	if(InUsingInstance == nullptr)
	{
		return;
	}

	if(UObsidianItemContainerComponent* Container = GetContainerFromOwnerForPosition(InUsingInstance->GetItemCurrentPosition()))
	{
		Container->ConsumeUsedItem(InUsingInstance, InCurrentStacks);
	}
}

bool UObsidianItemContainerComponent::ReplicateSubobjects(UActorChannel* InChannel, FOutBunch* InBunch, FReplicationFlags* InRepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(InChannel, InBunch, InRepFlags);

	for(UObsidianInventoryItemInstance* Instance : GetContainedItems())
	{
		if(IsValid(Instance))
		{
			WroteSomething |= InChannel->ReplicateSubobject(Instance, *InBunch, *InRepFlags);
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

bool UObsidianItemContainerComponent::HasOwnerAuthority(const ANSICHAR* InCallingFunction) const
{
	const AActor* Owner = GetOwner();
	if(Owner && Owner->HasAuthority())
	{
		return true;
	}

	UE_LOG(ObLogItemContainer, Warning, TEXT("No Authority in [%hs]"), InCallingFunction);
	return false;
}

void UObsidianItemContainerComponent::RegisterItemInstanceForReplication(UObsidianInventoryItemInstance* InInstance)
{
	if(InInstance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(InInstance);
	}
}

void UObsidianItemContainerComponent::UnregisterItemInstanceFromReplication(UObsidianInventoryItemInstance* InInstance)
{
	if(InInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(InInstance);
	}
}

int32 UObsidianItemContainerComponent::ClampStacksToAdd(const int32 InStacksAvailableToAdd, const int32 InStackToAddOverride)
{
	if(InStackToAddOverride == INDEX_NONE)
	{
		return InStacksAvailableToAdd;
	}
	return FMath::Clamp<int32>(FMath::Min<int32>(InStacksAvailableToAdd, InStackToAddOverride), 1, InStacksAvailableToAdd);
}

UObsidianInventoryItemInstance* UObsidianItemContainerComponent::PlaceItemDefinition(
	const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const FObsidianItemGeneratedData& InItemGeneratedData,
	const int32 InStackCount, const FObsidianItemPosition& InToPosition)
{
	UObsidianInventoryItemInstance* Instance = AddItemDefinitionToList(InItemDef, InItemGeneratedData, InStackCount, InToPosition);
	if(Instance == nullptr)
	{
		return nullptr;
	}

	Instance->AddItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, InStackCount);
	Instance->SetIdentified(UObsidianItemsFunctionLibrary::IsDefinitionIdentified(InItemDef.GetDefaultObject(), InItemGeneratedData));

	RegisterItemInstanceForReplication(Instance);
	return Instance;
}

void UObsidianItemContainerComponent::PlaceWholeItemInstance(UObsidianInventoryItemInstance* InInstanceToAdd,
	const FObsidianItemPosition& InToPosition)
{
	AddItemInstanceToList(InInstanceToAdd, InToPosition);
	RegisterItemInstanceForReplication(InInstanceToAdd);
}

UObsidianInventoryItemInstance* UObsidianItemContainerComponent::PlaceItemInstance(UObsidianInventoryItemInstance* InInstanceToAdd,
	const int32 InStacksToAdd, const FObsidianItemPosition& InToPosition, bool& bOutWholeItemPlaced)
{
	const int32 CurrentHeldItemStacks = InInstanceToAdd->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	bOutWholeItemPlaced = InStacksToAdd == CurrentHeldItemStacks;
	if(bOutWholeItemPlaced)
	{
		PlaceWholeItemInstance(InInstanceToAdd, InToPosition);
		return InInstanceToAdd;
	}

	// Only some of the stacks are placed, the rest stays on the provided Instance and a new one is created for the placed part.
	InInstanceToAdd->OverrideItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, CurrentHeldItemStacks - InStacksToAdd);
	const bool bCachedIdentified = InInstanceToAdd->IsItemIdentified();

	FObsidianItemGeneratedData CachedGeneratedData;
	UObsidianItemsFunctionLibrary::FillItemGeneratedData(CachedGeneratedData, InInstanceToAdd);

	UObsidianInventoryItemInstance* SplitInstance = AddItemDefinitionToList(InInstanceToAdd->GetItemDef(), CachedGeneratedData,
		InStacksToAdd, InToPosition);
	if(SplitInstance)
	{
		SplitInstance->AddItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, InStacksToAdd);
		SplitInstance->SetIdentified(bCachedIdentified);

		RegisterItemInstanceForReplication(SplitInstance);
	}
	return SplitInstance;
}

FObsidianAddingStacksResult UObsidianItemContainerComponent::AddStacksToItemFromInstance(
	UObsidianInventoryItemInstance* InAddingFromInstance, UObsidianInventoryItemInstance* InInstanceToAddTo,
	const int32 InStackToAddOverride)
{
	FObsidianAddingStacksResult Result = FObsidianAddingStacksResult();

	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(InAddingFromInstance == nullptr)
	{
		return Result;
	}

	const int32 AddingFromInstanceCurrentStacks = InAddingFromInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	Result.StacksLeft = AddingFromInstanceCurrentStacks;

	if(CanOwnerModifyContainerState() == false)
	{
		return Result;
	}

	if(UObsidianItemsFunctionLibrary::IsTheSameItem(InAddingFromInstance, InInstanceToAddTo) == false)
	{
		return Result;
	}

	int32 AmountThatCanBeAddedToInstance = UObsidianItemsFunctionLibrary::GetAmountOfStacksAllowedToAddToItem(GetOwner(),
		InAddingFromInstance, InInstanceToAddTo);
	if(AmountThatCanBeAddedToInstance <= 0)
	{
		return Result;
	}

	AmountThatCanBeAddedToInstance = ClampStacksToAdd(AmountThatCanBeAddedToInstance, InStackToAddOverride);

	const int32 OldStackCount = InInstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	InInstanceToAddTo->AddItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, AmountThatCanBeAddedToInstance);
	HandleItemStacksChanged(InInstanceToAddTo, OldStackCount);

	// AddingFromInstance is not held by this container, whoever holds it is responsible for reacting to the change.
	InAddingFromInstance->RemoveItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, AmountThatCanBeAddedToInstance);

	Result.AddedStacks = AmountThatCanBeAddedToInstance;
	Result.StacksLeft -= AmountThatCanBeAddedToInstance;
	Result.LastAddedToInstance = InInstanceToAddTo;
	Result.AddingStacksResult = AmountThatCanBeAddedToInstance == AddingFromInstanceCurrentStacks
		? EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded
		: EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded;
	return Result;
}

FObsidianAddingStacksResult UObsidianItemContainerComponent::AddStacksToItemFromDefinition(
	const TSubclassOf<UObsidianInventoryItemDefinition>& InAddingFromItemDef, const int32 InAddingFromItemDefCurrentStacks,
	UObsidianInventoryItemInstance* InInstanceToAddTo, const int32 InStackToAddOverride)
{
	FObsidianAddingStacksResult Result = FObsidianAddingStacksResult();
	Result.StacksLeft = InAddingFromItemDefCurrentStacks;

	if(HasOwnerAuthority(__FUNCTION__) == false)
	{
		return Result;
	}

	if(CanOwnerModifyContainerState() == false)
	{
		return Result;
	}

	if(UObsidianItemsFunctionLibrary::IsTheSameItem_WithDef(InInstanceToAddTo, InAddingFromItemDef) == false)
	{
		return Result;
	}

	int32 AmountThatCanBeAddedToInstance = UObsidianItemsFunctionLibrary::GetAmountOfStacksAllowedToAddToItem_WithDef(GetOwner(),
		InAddingFromItemDef, InAddingFromItemDefCurrentStacks, InInstanceToAddTo);
	if(AmountThatCanBeAddedToInstance <= 0)
	{
		return Result;
	}

	AmountThatCanBeAddedToInstance = ClampStacksToAdd(AmountThatCanBeAddedToInstance, InStackToAddOverride);

	const int32 OldStackCount = InInstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	InInstanceToAddTo->AddItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, AmountThatCanBeAddedToInstance);
	HandleItemStacksChanged(InInstanceToAddTo, OldStackCount);

	Result.StacksLeft -= AmountThatCanBeAddedToInstance;
	Result.AddedStacks = AmountThatCanBeAddedToInstance;
	Result.LastAddedToInstance = InInstanceToAddTo;
	Result.AddingStacksResult = AmountThatCanBeAddedToInstance == InAddingFromItemDefCurrentStacks
		? EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded
		: EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded;
	return Result;
}

FObsidianItemOperationResult UObsidianItemContainerComponent::TakeOutStacksFromItem(
	UObsidianInventoryItemInstance* InTakingFromInstance, const int32 InStacksToTake)
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

	if(InTakingFromInstance == nullptr)
	{
		return Result;
	}

	// The only valid number of stacks to take is in range [1, x - 1], taking the whole item out is just picking it up.
	// StacksToTake comes from the Client so it can't be trusted, anything outside this range would duplicate or void stacks.
	const int32 CurrentTakingFromInstanceStacks = InTakingFromInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	if(InStacksToTake < 1 || InStacksToTake >= CurrentTakingFromInstanceStacks)
	{
		return Result;
	}

	InTakingFromInstance->RemoveItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, InStacksToTake);
	HandleItemStacksChanged(InTakingFromInstance, CurrentTakingFromInstanceStacks);

	Result.bActionSuccessful = true;
	Result.AffectedInstance = InTakingFromInstance;
	Result.StacksLeft = InStacksToTake;
	return Result;
}

FObsidianItemOperationResult UObsidianItemContainerComponent::RemoveItemFromContainer(UObsidianInventoryItemInstance* InInstanceToRemove)
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

	if(InInstanceToRemove == nullptr)
	{
		UE_LOG(ObLogItemContainer, Error, TEXT("Passed InstanceToRemove is invalid in [%hs]"), __FUNCTION__);
		return Result;
	}

	RemoveItemInstanceFromList(InInstanceToRemove);
	UnregisterItemInstanceFromReplication(InInstanceToRemove);

	Result.bActionSuccessful = true;
	Result.AffectedInstance = InInstanceToRemove;
	return Result;
}

void UObsidianItemContainerComponent::ConsumeUsedItem(UObsidianInventoryItemInstance* InUsingInstance, const int32 InCurrentStacks)
{
	if(InUsingInstance == nullptr)
	{
		return;
	}

	if(InCurrentStacks > 1)
	{
		InUsingInstance->RemoveItemStackCount(ObsidianGameplayTags::Item::StackCount::Current, 1);
		HandleItemStacksChanged(InUsingInstance, InCurrentStacks);
		return;
	}

	RemoveItemInstanceFromList(InUsingInstance);
	UnregisterItemInstanceFromReplication(InUsingInstance);
}

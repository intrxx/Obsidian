// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ObsidianItemManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"
#include "Net/UnrealNetwork.h"

#include "CharacterComponents/ObsidianPlayerInputManager.h"
#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/Items/ObsidianDroppableItem.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianPickableInterface.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStash.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "Obsidian/ObsidianLogCategories.h"
#include "UI/InventoryItems/Items/ObsidianDraggedItem.h"


UObsidianItemManagerComponent::UObsidianItemManagerComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UObsidianItemManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, DraggedItem, COND_OwnerOnly);
}

void UObsidianItemManagerComponent::TickComponent(float InDeltaTime, ELevelTick InTickType,
	FActorComponentTickFunction* InThisTickFunction)
{
	Super::TickComponent(InDeltaTime, InTickType, InThisTickFunction);

	if(bDraggingItem)
	{
		DragItem();
	}
}

FDraggedItem UObsidianItemManagerComponent::GetDraggedItem()
{
	return DraggedItem;
}

bool UObsidianItemManagerComponent::IsDraggingAnItem() const
{
	return bDraggingItem;
}

bool UObsidianItemManagerComponent::CanDropItem() const
{
	return IsDraggingAnItem() && bItemAvailableForDrop;
}

bool UObsidianItemManagerComponent::DidJustDroppedItem() const
{
	return bJustDroppedItem;
}

void UObsidianItemManagerComponent::SetJustDroppedItem(const bool bInJustDroppedItem)
{
	bJustDroppedItem = bInJustDroppedItem;
}

bool UObsidianItemManagerComponent::DropItem()
{
	if(CanDropItem() == false)
	{
		return false;
	}
	ServerHandleDroppingItem();
	return true;
}

void UObsidianItemManagerComponent::ServerAddItemToInventoryAtSlot_Implementation(const FIntPoint& InSlotPosition,
	const bool bInShiftDown)
{
	if(DraggedItem.IsEmpty())
	{
		UE_LOG(ObLogItemManager, Error, TEXT("Tried to add Inventory Item to the Inventory at specific slot"
								   " but the Dragged Item is Empty in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const int32 StacksToAddOverride = bInShiftDown ? 1 : -1;
	
	if(UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		const int32 CurrentStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
		const FObsidianItemOperationResult Result = InventoryComponent->AddItemInstanceToSpecificSlot(Instance,
			InSlotPosition, StacksToAddOverride);
		
		UpdateDraggedItem(Result, CurrentStackCount, Controller);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		const int32 CachedStacks = DraggedItem.GeneratedData.GetStackCount();
		const FObsidianItemOperationResult Result = InventoryComponent->AddItemDefinitionToSpecifiedSlot(ItemDef,
			InSlotPosition, DraggedItem.GeneratedData, StacksToAddOverride);

		UpdateDraggedItem(Result, CachedStacks, Controller);
	}
}

void UObsidianItemManagerComponent::ServerAddStacksFromDraggedItemToInventoryItemAtSlot_Implementation(
	const FIntPoint& InSlotPosition, const int32 InStacksToAddOverride)
{
	if(DraggedItem.IsEmpty())
	{
		UE_LOG(ObLogItemManager, Error, TEXT("Tried to add Inventory Item to the Inventory at specific slot"
								   " but the Dragged Item is Empty in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianInventoryItemInstance* Instance = DraggedItem.Instance;
	if(Instance && Instance->IsStackable())
	{
		const int32 PreviousStacks = Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
		const FObsidianAddingStacksResult AddingStacksResult = InventoryComponent->TryAddingStacksToSpecificSlotWithInstance(
			Instance, InSlotPosition, InStacksToAddOverride);
		
		UpdateDraggedItem(AddingStacksResult, PreviousStacks, Controller);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
		if(DefaultObject && DefaultObject->IsStackable())
		{
			const int32 CachedStacks = DraggedItem.GeneratedData.GetStackCount();
			const FObsidianAddingStacksResult AddingStacksResult = InventoryComponent->TryAddingStacksToSpecificSlotWithItemDef(
				ItemDef, CachedStacks, InSlotPosition, InStacksToAddOverride);

			UpdateDraggedItem(AddingStacksResult, CachedStacks, Controller);
		}
	}
}

void UObsidianItemManagerComponent::ServerTakeoutFromInventoryItem_Implementation(const FIntPoint& InSlotPosition,
	const int32 InStacksToTake)
{
	if(DraggedItem.IsEmpty() == false)
	{
		UE_LOG(ObLogItemManager, Warning, TEXT("Already dragging an Item, it would be lost in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* ItemInstance = InventoryComponent->GetItemInstanceAtLocation(InSlotPosition);
	if(ItemInstance == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("ItemInstance is null in [%hs]"), __FUNCTION__);
		return;
	}

	const FObsidianItemOperationResult Result = InventoryComponent->TakeOutFromItemInstance(ItemInstance, InStacksToTake);
	const UObsidianInventoryItemInstance* AffectedInstance = Result.AffectedInstance;
	if(AffectedInstance == nullptr || Result.bActionSuccessful == false)
	{
		return;
	}

	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = AffectedInstance->GetItemDef();
	if(ItemDef == nullptr)
	{
		return;
	}

	//TODO(intrxx) In this case this are actually StacksToTake, maybe create another struct to reflect that?
	DraggedItem = FDraggedItem(ItemDef, Result.StacksLeft);
	StartDraggingItem(Controller);
}

void UObsidianItemManagerComponent::ServerReplaceItemAtInventorySlot_Implementation(
	const FIntPoint& InClickedItemPosition, const FIntPoint& InClickedGridPosition)
{
	const AController* Controller = Cast<AController>(GetOwner());
	if (Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if (InventoryComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	if (InventoryComponent->CanOwnerModifyInventoryState() == false)
	{
		return;
	}
	
	const FDraggedItem CachedDraggedItem = DraggedItem;
	DraggedItem.Clear();
	StopDraggingItem(Controller);
	
	ServerGrabInventoryItemToCursor(InClickedItemPosition);

	bool bSuccess = false;
	if (UObsidianInventoryItemInstance* Instance = CachedDraggedItem.Instance)
	{
		bSuccess = InventoryComponent->AddItemInstanceToSpecificSlot(Instance, InClickedGridPosition);
	}
	else if (const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = CachedDraggedItem.ItemDef)
	{
		bSuccess = InventoryComponent->AddItemDefinitionToSpecifiedSlot(ItemDef, InClickedGridPosition,
			CachedDraggedItem.GeneratedData);
	}
	
	if (bSuccess == false)
	{
		ServerAddItemToInventoryAtSlot(InClickedItemPosition, false);
		DraggedItem = CachedDraggedItem;
		StartDraggingItem(Controller);
	}
}

void UObsidianItemManagerComponent::ServerGrabDroppableItemToCursor_Implementation(AObsidianDroppableItem* InItemToPickup)
{
	if(DraggedItem.IsEmpty() == false)
	{
		UE_LOG(ObLogItemManager, Warning, TEXT("Already dragging an Item, it would be lost in [%hs]"), __FUNCTION__);
		return;
	}

	if(InItemToPickup == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("ItemToPickup is null in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	if (VerifyPickupRange(InItemToPickup) == false)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("[%hs]: Item is too far to be picked up!"), __FUNCTION__);
		return;
	}

	// if(HandlePickUpIfItemOutOfRange(ItemToPickup, EObsidianItemPickUpType::PickUpToDrag))
	// {
	// 	return;
	// }
	
	const FObsidianPickupTemplate Template = InItemToPickup->GetPickupTemplateFromPickupContent();
	if(Template.IsValid()) // We are grabbing Item Template
	{
		DraggedItem = FDraggedItem(Template.ItemDef, Template.ItemGeneratedData);
		InItemToPickup->UpdateDroppedItemStacks(0);
		
		StartDraggingItem(Controller);
		return;
	}

	const FObsidianPickupInstance Instance = InItemToPickup->GetPickupInstanceFromPickupContent();
	if(Instance.IsValid()) // We are grabbing Item Instance
	{
		DraggedItem = FDraggedItem(Instance.Item);
		InItemToPickup->UpdateDroppedItemStacks(0);
		
		StartDraggingItem(Controller);
		return;
	}

	checkf(false, TEXT("Provided ItemToPickup has no Instance nor Template to pick up,"
					" this is bad and should not happen."))
}

void UObsidianItemManagerComponent::ServerGrabInventoryItemToCursor_Implementation(const FIntPoint& InSlotPosition)
{
	if(DraggedItem.IsEmpty() == false)
	{
		UE_LOG(ObLogItemManager, Warning, TEXT("Already dragging an Item, it would be lost in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = InventoryComponent->GetItemInstanceAtLocation(InSlotPosition);
	if(InstanceToGrab == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("InstanceToGrab is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	if(InventoryComponent->RemoveItemInstance(InstanceToGrab) == false)
	{
		return;
	}
	
	DraggedItem = FDraggedItem(InstanceToGrab);

	StartDraggingItem(Controller);
}

void UObsidianItemManagerComponent::ServerPickupItem_Implementation(AObsidianDroppableItem* InItemToPickup)
{
	if(InItemToPickup == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("ItemToPickup is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("Controller is null in [%hs]"), __FUNCTION__);
		return;
	}

	if (VerifyPickupRange(InItemToPickup) == false)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("[%hs]: Item is too far to be picked up!"), __FUNCTION__);
		return;
	}
	
	// if(HandlePickUpIfItemOutOfRange(ItemToPickup, EObsidianItemPickUpType::AutomaticPickUp))
	// {
	// 	return;
	// }
	
	const FObsidianPickupTemplate Template = InItemToPickup->GetPickupTemplateFromPickupContent();
	if(Template.IsValid())
	{
		const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = Template.ItemDef;
		if(ItemDef == nullptr)
		{
			UE_LOG(ObLogItemManager, Error, TEXT("ItemDef is null in [%hs]"), __FUNCTION__);
			return;
		}

		if(const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject())
		{
			if(DefaultObject->IsEquippable())
			{
				UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
				if(EquipmentComponent == nullptr)
				{
					UE_LOG(ObLogItemManager, Error, TEXT("OwnerEquipmentComponent is null in [%hs]"),
						__FUNCTION__);
					return;
				}
			
				if(EquipmentComponent->AutomaticallyEquipItem(ItemDef, Template.ItemGeneratedData))
				{
					InItemToPickup->DestroyDroppedItem();
					return;
				}
			}
		}

		UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
		if(InventoryComponent == nullptr)
		{
			UE_LOG(ObLogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
			return;
		}
		
		if(const FObsidianItemOperationResult& Result = InventoryComponent->AddItemDefinition(ItemDef,
			Template.ItemGeneratedData))
		{
			InItemToPickup->UpdateDroppedItemStacks(Result.StacksLeft);
		}
		
		return;
	}

	const FObsidianPickupInstance Instance = InItemToPickup->GetPickupInstanceFromPickupContent();
	if(Instance.IsValid())
	{
		UObsidianInventoryItemInstance* ItemInstance = Instance.Item;
		if(ItemInstance == nullptr)
		{
			UE_LOG(ObLogItemManager, Error, TEXT("ItemInstance is null in [%hs]"), __FUNCTION__);
			return;
		}

		if(ItemInstance->IsItemEquippable())
		{
			UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
			if(EquipmentComponent == nullptr)
			{
				UE_LOG(ObLogItemManager, Error, TEXT("OwnerEquipmentComponent is null in [%hs]"), __FUNCTION__);
				return;
			}
			
			if(EquipmentComponent->AutomaticallyEquipItem(ItemInstance))
			{
				InItemToPickup->DestroyDroppedItem();
				return;
			}
		}

		UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
		if(InventoryComponent == nullptr)
		{
			UE_LOG(ObLogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
			return;
		}


		const int32 CurrentStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
		const FObsidianItemOperationResult Result = InventoryComponent->AddItemInstance(ItemInstance);
		if(CurrentStacks != Result.StacksLeft)
		{
			InItemToPickup->UpdateDroppedItemStacks(Result.StacksLeft);
		}
		return;
	}
}

void UObsidianItemManagerComponent::ServerTransferItemToPlayerStash_Implementation(
	const FIntPoint& InFromInventoryPosition, const FGameplayTag& InToStashTab)
{
	if(IsOwnerInPlayerStashRange() == false)
	{
		UE_LOG(ObLogItemManager, Warning, TEXT("[%hs]: Owner is not in range of any Player Stash!"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if (Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if (PlayerStashComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerPlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if (InventoryComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = InventoryComponent->GetItemInstanceAtLocation(InFromInventoryPosition);
	if (InstanceToGrab == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("InstanceToGrab is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	if (PlayerStashComponent->CanFitInstanceInStashTab(InstanceToGrab->GetItemGridSpan(),
		InstanceToGrab->GetItemCategoryTag(), InstanceToGrab->GetItemBaseTypeTag(), InToStashTab))
	{
		if (InventoryComponent->RemoveItemInstance(InstanceToGrab))
		{
			PlayerStashComponent->AddItemInstance(InstanceToGrab, InToStashTab);
		}
	}
}

void UObsidianItemManagerComponent::ServerEquipItemAtSlot_Implementation(const FGameplayTag& InSlotTag)
{
	if(DraggedItem.IsEmpty())
	{
		UE_LOG(ObLogItemManager, Error, TEXT("Tried to add Inventory Item to the Inventory at specific slot"
									 " but the Dragged Item is Empty in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
	if(EquipmentComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerEquipmentComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	if(UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		if(EquipmentComponent->EquipItemToSpecificSlot(Instance, InSlotTag))
		{
			DraggedItem.Clear();
			StopDraggingItem(Controller);
		}
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		if(EquipmentComponent->EquipItemToSpecificSlot(ItemDef, InSlotTag, DraggedItem.GeneratedData))
		{
			DraggedItem.Clear();
			StopDraggingItem(Controller);
		}
	}
}

void UObsidianItemManagerComponent::ServerGrabEquippedItemToCursor_Implementation(const FGameplayTag& InSlotTag)
{
	if(DraggedItem.IsEmpty() == false)
	{
		UE_LOG(ObLogItemManager, Warning, TEXT("Already dragging an Item, it would be lost in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
	if(EquipmentComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = EquipmentComponent->GetEquippedInstanceAtSlot(InSlotTag);
	if(InstanceToGrab == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("InstanceToGrab is null in [%hs]"), __FUNCTION__);
		return;
	}

	if(EquipmentComponent->UnequipItem(InstanceToGrab) == false)
	{
		return;
	}

	DraggedItem = FDraggedItem(InstanceToGrab);

	StartDraggingItem(Controller);
}

void UObsidianItemManagerComponent::ServerReplaceItemAtEquipmentSlot_Implementation(const FGameplayTag& InSlotTag,
	const FGameplayTag& InEquipSlotTagOverride)
{
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
	if(EquipmentComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	if(EquipmentComponent->CanOwnerModifyEquipmentState() == false)
	{
		return;
	}

	const FDraggedItem CachedDraggedItem = DraggedItem;		
	DraggedItem.Clear();
	StopDraggingItem(Controller);
	
	ServerGrabEquippedItemToCursor(InSlotTag);

	bool bSuccess = false;
	if(UObsidianInventoryItemInstance* Instance = CachedDraggedItem.Instance)
	{
		bSuccess = EquipmentComponent->ReplaceItemAtSpecificSlot(Instance, InSlotTag, InEquipSlotTagOverride);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = CachedDraggedItem.ItemDef)
	{
		bSuccess = EquipmentComponent->ReplaceItemAtSpecificSlot(ItemDef, InSlotTag, CachedDraggedItem.GeneratedData,
			InEquipSlotTagOverride);
	}
	
	if(bSuccess == false)
	{
		ServerEquipItemAtSlot(InSlotTag);
		DraggedItem = CachedDraggedItem;
		StartDraggingItem(Controller);
	}
}

void UObsidianItemManagerComponent::ServerWeaponSwap_Implementation()
{
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
	if(EquipmentComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	EquipmentComponent->WeaponSwap();
}

void UObsidianItemManagerComponent::ServerAddItemToStashTabAtSlot_Implementation(
	const FObsidianItemPosition& InAtPosition, const bool bInShiftDown)
{
	if(IsOwnerInPlayerStashRange() == false)
	{
		UE_LOG(ObLogItemManager, Warning, TEXT("[%hs]: Owner is not in range of any Player Stash!"), __FUNCTION__);
		return;
	}

	if(DraggedItem.IsEmpty())
	{
		UE_LOG(ObLogItemManager, Error, TEXT("Tried to add Inventory Item to the Inventory at specific slot"
									 " but the Dragged Item is Empty in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if(PlayerStashComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerPlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const int32 StacksToAddOverride = bInShiftDown ? 1 : -1;
	
	if(UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		const int32 CurrentStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
		const FObsidianItemOperationResult Result = PlayerStashComponent->AddItemInstanceToSpecificSlot(Instance,
			InAtPosition, StacksToAddOverride);
		
		UpdateDraggedItem(Result, CurrentStackCount, Controller);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		const int32 CachedStacks = DraggedItem.GeneratedData.GetStackCount();
		const FObsidianItemOperationResult Result = PlayerStashComponent->AddItemDefinitionToSpecifiedSlot(ItemDef,
			InAtPosition, DraggedItem.GeneratedData, StacksToAddOverride);

		UpdateDraggedItem(Result, CachedStacks, Controller);
	}
}

void UObsidianItemManagerComponent::ServerAddStacksFromDraggedItemToStashedItemAtSlot_Implementation(
	const FObsidianItemPosition& InAtPosition, const int32 InStacksToAddOverride)
{
	if(IsOwnerInPlayerStashRange() == false)
	{
		UE_LOG(ObLogItemManager, Warning, TEXT("[%hs]: Owner is not in range of any Player Stash!"), __FUNCTION__);
		return;
	}

	if(DraggedItem.IsEmpty())
	{
		UE_LOG(ObLogItemManager, Error, TEXT("Tried to add Stacks from Dragged Item,"
									 " but Dragged Item is empty [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if(PlayerStashComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerPlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianInventoryItemInstance* Instance = DraggedItem.Instance;
	if(Instance && Instance->IsStackable())
	{
		const int32 CurrentStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
		const FObsidianAddingStacksResult AddingStacksResult = PlayerStashComponent->TryAddingStacksToSpecificSlotWithInstance(
			Instance, InAtPosition, InStacksToAddOverride);
		
		UpdateDraggedItem(AddingStacksResult, CurrentStackCount, Controller);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
		if(DefaultObject && DefaultObject->IsStackable())
		{
			const int32 CurrentStackCount = DraggedItem.GeneratedData.GetStackCount();
			const FObsidianAddingStacksResult AddingStacksResult = PlayerStashComponent->TryAddingStacksToSpecificSlotWithItemDef(
				ItemDef, CurrentStackCount, InAtPosition, InStacksToAddOverride);
			
			UpdateDraggedItem(AddingStacksResult, CurrentStackCount, Controller);
		}
	}
}
									
void UObsidianItemManagerComponent::ServerGrabStashedItemToCursor_Implementation(
	const FObsidianItemPosition& InFromPosition)
{
	if(DraggedItem.IsEmpty() == false)
	{
		UE_LOG(ObLogItemManager, Warning, TEXT("Already dragging an Item, it would be lost in [%hs]"), __FUNCTION__);
		return;
	}

	if(IsOwnerInPlayerStashRange() == false)
	{
		UE_LOG(ObLogItemManager, Warning, TEXT("[%hs]: Owner is not in range of any Player Stash!"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if(PlayerStashComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerPlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = PlayerStashComponent->GetItemInstanceFromTabAtPosition(InFromPosition);
	if(InstanceToGrab == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("InstanceToGrab is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	if(PlayerStashComponent->RemoveItemInstance(InstanceToGrab) == false)
	{
		return;
	}
	
	DraggedItem = FDraggedItem(InstanceToGrab);
	StartDraggingItem(Controller);
}

void UObsidianItemManagerComponent::ServerTransferItemToInventory_Implementation(
	const FObsidianItemPosition& InFromStashPosition)
{
	if(IsOwnerInPlayerStashRange() == false)
	{
		UE_LOG(ObLogItemManager, Warning, TEXT("[%hs]: Owner is not in range of any Player Stash!"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if (Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if (PlayerStashComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerPlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if (InventoryComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = PlayerStashComponent->GetItemInstanceFromTabAtPosition(InFromStashPosition);
	if (InstanceToGrab == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("InstanceToGrab is null in [%hs]"), __FUNCTION__);
		return;
	}

	if (InventoryComponent->CanFitItemInstance(InstanceToGrab))
	{
		if (PlayerStashComponent->RemoveItemInstance(InstanceToGrab))
		{
			InventoryComponent->AddItemInstance(InstanceToGrab);
		}
	}
}

void UObsidianItemManagerComponent::ServerReplaceItemAtStashPosition_Implementation(
	const FObsidianItemPosition& InAtStashPosition)
{
	if(IsOwnerInPlayerStashRange() == false)
	{
		UE_LOG(ObLogItemManager, Warning, TEXT("[%hs]: Owner is not in range of any Player Stash!"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if (Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if (PlayerStashComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerPlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	if (PlayerStashComponent->CanOwnerModifyPlayerStashState() == false)
	{
		return;
	}
	
	const FDraggedItem CachedDraggedItem = DraggedItem;
	DraggedItem.Clear();
	StopDraggingItem(Controller);
	
	ServerGrabStashedItemToCursor(InAtStashPosition);

	bool bSuccess = false;
	if (UObsidianInventoryItemInstance* Instance = CachedDraggedItem.Instance)
	{
		bSuccess = PlayerStashComponent->AddItemInstanceToSpecificSlot(Instance, InAtStashPosition);
	}
	else if (const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = CachedDraggedItem.ItemDef)
	{
		bSuccess = PlayerStashComponent->AddItemDefinitionToSpecifiedSlot(ItemDef, InAtStashPosition,
			CachedDraggedItem.GeneratedData);
	}
	
	if (bSuccess == false)
	{
		ServerAddItemToStashTabAtSlot(InAtStashPosition, false);
		DraggedItem = CachedDraggedItem;
		StartDraggingItem(Controller);
	}
}

void UObsidianItemManagerComponent::ServerTakeoutFromStashedItem_Implementation(
	const FObsidianItemPosition& InAtStashPosition, const int32 InStacksToTake)
{
	if(DraggedItem.IsEmpty() == false)
	{
		UE_LOG(ObLogItemManager, Warning, TEXT("Already dragging an Item, it would be lost in [%hs]"), __FUNCTION__);
		return;
	}

	if(IsOwnerInPlayerStashRange() == false)
	{
		UE_LOG(ObLogItemManager, Warning, TEXT("[%hs]: Owner is not in range of any Player Stash!"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if(PlayerStashComponent == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerPlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* ItemInstance = PlayerStashComponent->GetItemInstanceFromTabAtPosition(InAtStashPosition);
	if(ItemInstance == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("ItemInstance is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const FObsidianItemOperationResult Result = PlayerStashComponent->TakeOutFromItemInstance(ItemInstance, InStacksToTake);
	const UObsidianInventoryItemInstance* AffectedInstance = Result.AffectedInstance;
	if(AffectedInstance == nullptr || Result.bActionSuccessful == false)
	{
		return;
	}

	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = AffectedInstance->GetItemDef();
	if(ItemDef == nullptr)
	{
		return;
	}

	//TODO(intrxx) In this case this are actually StacksToTake, maybe create another struct to reflect that?
	DraggedItem = FDraggedItem(ItemDef, Result.StacksLeft);
	StartDraggingItem(Controller);
}

bool UObsidianItemManagerComponent::ReplicateSubobjects(UActorChannel* InChannel, FOutBunch* InBunch,
	FReplicationFlags* InRepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(InChannel, InBunch, InRepFlags);

	UObsidianInventoryItemInstance* Instance = DraggedItem.Instance;
	if(Instance && IsValid(Instance))
	{
		WroteSomething |= InChannel->ReplicateSubobject(Instance, *InBunch, *InRepFlags);
	}
		
	return WroteSomething;
}

void UObsidianItemManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing UObsidianInventoryItemInstance
	if(IsUsingRegisteredSubObjectList() && !DraggedItem.IsEmpty())
	{
		UObsidianInventoryItemInstance* Instance = DraggedItem.Instance;
		if(IsValid(Instance))
		{
			AddReplicatedSubObject(Instance);
		}
	}
}

void UObsidianItemManagerComponent::OnRep_DraggedItem(const FDraggedItem& InOldDraggedItem)
{
	if(DraggedItem.IsEmpty() && bDraggingItem) // We cleared Dragged Item, so we should no longer drag it
	{
		const AController* Controller = Cast<AController>(GetOwner());
		if(Controller == nullptr)
		{
			UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
			return;
		}
		StopDraggingItem(Controller);
	}
	else if(!DraggedItem.IsEmpty() && !bDraggingItem || DraggedItemWasReplaced(InOldDraggedItem))  // We got new Item to drag
	{
		const AController* Controller = Cast<AController>(GetOwner());
		if(Controller == nullptr)
		{
			UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
			return;
		}
		StartDraggingItem(Controller);
	}
	else if(DraggedItem.GeneratedData.GetStackCount() > 0) // We are dragging an item but the stacks changed. //TODO(intrxx) Why just "Stacks > 0"?
	{
		if(DraggedItemWidget)
		{
			DraggedItemWidget->UpdateStackCount(DraggedItem.GeneratedData.GetStackCount());
		}
	}
	//TODO(intrxx) I don't think I need to account for Rarity and Affixes changes but check in later 
}

void UObsidianItemManagerComponent::DragItem() const
{
	const APlayerController* PC = Cast<APlayerController>(GetOwner());
	if(PC == nullptr)
	{
		return;
	}

	float LocationX = 0.0f;
	float LocationY = 0.0f;
	if(DraggedItemWidget && PC->GetMousePosition(LocationX, LocationY))
	{
		const FVector2D ViewportPosition = FVector2D(LocationX, LocationY);
		DraggedItemWidget->SetPositionInViewport(ViewportPosition);
	}
}

void UObsidianItemManagerComponent::StartDraggingItem(const AController* InController)
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}
	
	if(InController && !InController->IsLocalController())
	{
		return;
	}

	if(DraggedItemWidget)
	{
		DraggedItemWidget->RemoveFromParent();
	}
	
	checkf(DraggedItemWidgetClass, TEXT("DraggedItemWidgetClass is invalid in [%hs] please fill it on "
									 "ObsidianDroppableItem Instance."), __FUNCTION__);
	UObsidianDraggedItem* Item = CreateWidget<UObsidianDraggedItem>(World, DraggedItemWidgetClass);

	bool bInitialized = false;
	if(UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		Item->InitializeItemWidgetWithItemInstance(Instance);
		bInitialized = true;
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		Item->InitializeItemWidgetWithItemDef(ItemDef, DraggedItem.GeneratedData);
		bInitialized = true;
	}
	checkf(bInitialized, TEXT("Item was not initialized with neither Instance nor ItemDef,"
						   " this is bad and should not happen."));
	
	Item->AddToViewport();
	
	DraggedItemWidget = Item;
	bDraggingItem = true;

	TWeakObjectPtr<UObsidianItemManagerComponent> WeakThis(this);
	World->GetTimerManager().SetTimerForNextTick([WeakThis]()
	{
		if(WeakThis.IsValid())
		{
			WeakThis->bItemAvailableForDrop = true;
		}
	});

	OnStartDraggingItemDelegate.Broadcast(DraggedItem);
}

void UObsidianItemManagerComponent::StopDraggingItem(const AController* InController)
{
	if(InController && !InController->IsLocalController())
	{
		return;
	}
	
	if(DraggedItemWidget)
	{
		DraggedItemWidget->RemoveFromParent();
	}
	
	bDraggingItem = false;
	DraggedItemWidget = nullptr;
	bItemAvailableForDrop = false;

	bJustDroppedItem = true;

	OnStopDraggingItemDelegate.Broadcast();
}

void UObsidianItemManagerComponent::UpdateDraggedItem(const FObsidianItemOperationResult& InOperationResult,
	const int32 InCachedNumberOfStack, const AController* InForController)
{
	if(InCachedNumberOfStack != InOperationResult.StacksLeft)
	{
		if(InOperationResult.bActionSuccessful)
		{
			DraggedItem.Clear();
			StopDraggingItem(InForController);
			return;
		}
		UpdateStacksOnDraggedItemWidget(InOperationResult.StacksLeft);
		DraggedItem.GeneratedData.SetStackCount(InOperationResult.StacksLeft);
	}
}

void UObsidianItemManagerComponent::UpdateDraggedItem(const FObsidianAddingStacksResult& InOperationResult,
	const int32 InCachedNumberOfStack, const AController* InForController)
{
	if(InCachedNumberOfStack != InOperationResult.StacksLeft)
	{
		if(InOperationResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded)
		{
			DraggedItem.Clear();
			StopDraggingItem(InForController);
		}
		else if(InOperationResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded)
		{
			UpdateStacksOnDraggedItemWidget(InOperationResult.StacksLeft);
			DraggedItem.GeneratedData.SetStackCount(InOperationResult.StacksLeft);
		}
	}
}

bool UObsidianItemManagerComponent::DraggedItemWasReplaced(const FDraggedItem& InOldDraggedItem) const
{
	if(InOldDraggedItem.Instance && InOldDraggedItem.Instance != DraggedItem.Instance)
	{
		return true;
	}

	if(InOldDraggedItem.ItemDef && DraggedItem.Instance)
	{
		return true;
	}
	
	return false;
}

void UObsidianItemManagerComponent::UpdateStacksOnDraggedItemWidget(const int32 InStacks)
{
	if(DraggedItemWidget)
	{
		DraggedItemWidget->UpdateStackCount(InStacks);
	}
}

bool UObsidianItemManagerComponent::VerifyPickupRange(const AObsidianDroppableItem* InItemToPickUp) const
{
	return IsOwnerInInteractionRange(InItemToPickUp, ObsidianPlayerInputStatics::InteractionRadius);
}

bool UObsidianItemManagerComponent::IsOwnerInPlayerStashRange() const
{
	const UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return false;
	}

	for(TActorIterator<AObsidianPlayerStash> It(World); It; ++It)
	{
		AObsidianPlayerStash* PlayerStash = *It;
		if(IsValid(PlayerStash) == false)
		{
			continue;
		}

		const float StashInteractionRadius = PlayerStash->GetInteractionRadius();
		if(IsOwnerInInteractionRange(PlayerStash, StashInteractionRadius == 0.0f ? 
			ObsidianPlayerInputStatics::InteractionRadius : StashInteractionRadius))
		{
			return true;
		}
	}
	return false;
}

bool UObsidianItemManagerComponent::IsOwnerInInteractionRange(const AActor* InInteractionActor, const float InInteractionRadius) const
{
	if(InInteractionActor == nullptr)
	{
		return false;
	}

	const APlayerController* PC = Cast<APlayerController>(GetOwner());
	if(PC == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("ObsidianPC is null in [%hs]"), __FUNCTION__);
		return false;
	}

	const ACharacter* OwnerCharacter = PC->GetCharacter();
	if (OwnerCharacter == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwnerCharacter is null in [%hs]"), __FUNCTION__);
		return false;
	}

	const float DistanceToActorSquared = FVector::DistSquared2D(OwnerCharacter->GetActorLocation(), InInteractionActor->GetActorLocation());
	return DistanceToActorSquared <= FMath::Square(InInteractionRadius + ObsidianPlayerInputStatics::InteractionRangeTolerance);
}

void UObsidianItemManagerComponent::ServerHandleDroppingItem_Implementation()
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}

	if(DraggedItem.IsEmpty())
	{
		UE_LOG(ObLogItemManager, Warning, TEXT("Tried to drop an Item but the Dragged Item is Empty in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	ACharacter* OwningCharacter = Controller->GetCharacter();
	if (OwningCharacter == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("OwningCharacter is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const FVector OwnerLocation = OwningCharacter->GetActorLocation();

#if 0 // https://github.com/intrxx/Obsidian/commit/e3eda3899a1b39ec1952221a24bce0b40b7be769
	const FVector ClickedLocation = CursorHit.Location;
	const float ItemDropLocation = CachedItemDropLocation == FVector::Zero()
			? FVector::Distance(OwnerLocation, ClickedLocation)
			: FVector::Distance(OwnerLocation, CachedItemDropLocation);
	if(false) // TODO Check if there is no room in the drop space for dropping the item, then move character to some other location at cached destination
	{
		if(CursorHit.bBlockingHit)
		{
			CachedDestination = ClickedLocation;
		}
		CachedItemDropLocation = CachedDestination;
		AutoRunToClickedLocation();
		return;
	}
	
	CachedItemDropLocation = FVector::Zero();
#endif
	
	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(World);
	if(NavigationSystem == nullptr)
	{
		UE_LOG(ObLogItemManager, Error, TEXT("NavigationSystem is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	//TODO(intrxx) Bias towards Actors forward Vector
	FNavLocation RandomPointLocation;
	if(NavigationSystem->GetRandomPointInNavigableRadius(OwnerLocation, DropRadius, RandomPointLocation) == false)
	{
		RandomPointLocation.Location = OwnerLocation;
	}
	
	FHitResult GroundTraceResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwningCharacter);
	const FVector GroundTraceEndLocation = FVector(RandomPointLocation.Location.X, RandomPointLocation.Location.Y,
		RandomPointLocation.Location.Z - 300.0f);
	World->LineTraceSingleByChannel(GroundTraceResult, RandomPointLocation.Location, GroundTraceEndLocation,
		ECC_Visibility, QueryParams);

	FRotator ItemRotation = FRotator::ZeroRotator;
	FVector ItemLocation = RandomPointLocation.Location;
	if(GroundTraceResult.bBlockingHit) // We are able to align the item to the ground better
	{
		FVector RandomisedRotationVector = FMath::VRand().GetSafeNormal();
		ItemRotation = UKismetMathLibrary::MakeRotFromZY(GroundTraceResult.ImpactNormal, RandomisedRotationVector);
		ItemLocation = GroundTraceResult.Location;
	}
	
	const FTransform ItemSpawnTransform = FTransform(ItemRotation, ItemLocation,
		FVector(1.0f, 1.0f, 1.0f));
	AObsidianDroppableItem* Item = World->SpawnActorDeferred<AObsidianDroppableItem>(
		AObsidianDroppableItem::StaticClass(), ItemSpawnTransform);
	Item->InitializeItem(DraggedItem);
	Item->FinishSpawning(ItemSpawnTransform);

	DraggedItem.Clear();

	StopDraggingItem(Controller);
}

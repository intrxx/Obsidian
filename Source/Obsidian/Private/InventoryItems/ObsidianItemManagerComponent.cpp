// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ObsidianItemManagerComponent.h"

#include <Net/UnrealNetwork.h>
#include <Engine/ActorChannel.h>
#include <Kismet/KismetMathLibrary.h>
#include <NavigationSystem.h>
#include <GameFramework/Character.h>

#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianPickableInterface.h"
#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "InventoryItems/Items/ObsidianDroppableItem.h"
#include "UI/Inventory/Items/ObsidianDraggedItem.h"

DEFINE_LOG_CATEGORY(LogItemManager)

UObsidianItemManagerComponent::UObsidianItemManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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

void UObsidianItemManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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

void UObsidianItemManagerComponent::ServerAddItemToInventoryAtSlot_Implementation(const FIntPoint& SlotPosition,
	const bool bShiftDown)
{
	if(DraggedItem.IsEmpty())
	{
		UE_LOG(LogItemManager, Error, TEXT("Tried to add Inventory Item to the Inventory at specific slot"
								   " but the Dragged Item is Empty in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const int32 StacksToAddOverride = bShiftDown ? 1 : -1;
	
	if(UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		const int32 CurrentStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		const FObsidianItemOperationResult Result = InventoryComponent->AddItemInstanceToSpecificSlot(Instance,
			SlotPosition, StacksToAddOverride);
		
		UpdateDraggedItem(Result, CurrentStackCount, Controller);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		const int32 CachedStacks = DraggedItem.GeneratedData.AvailableStackCount;
		const FObsidianItemOperationResult Result = InventoryComponent->AddItemDefinitionToSpecifiedSlot(ItemDef,
			SlotPosition, DraggedItem.GeneratedData, StacksToAddOverride);

		UpdateDraggedItem(Result, CachedStacks, Controller);
	}
}

void UObsidianItemManagerComponent::ServerAddStacksFromDraggedItemToInventoryItemAtSlot_Implementation(
	const FIntPoint& SlotPosition, const int32 StacksToAddOverride)
{
	if(DraggedItem.IsEmpty())
	{
		UE_LOG(LogItemManager, Error, TEXT("Tried to add Inventory Item to the Inventory at specific slot"
								   " but the Dragged Item is Empty in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianInventoryItemInstance* Instance = DraggedItem.Instance;
	if(Instance && Instance->IsStackable())
	{
		const int32 PreviousStacks = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		const FObsidianAddingStacksResult AddingStacksResult = InventoryComponent->TryAddingStacksToSpecificSlotWithInstance(
			Instance, SlotPosition, StacksToAddOverride);
		
		UpdateDraggedItem(AddingStacksResult, PreviousStacks, Controller);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
		if(DefaultObject && DefaultObject->IsStackable())
		{
			const int32 CachedStacks = DraggedItem.GeneratedData.AvailableStackCount;
			const FObsidianAddingStacksResult AddingStacksResult = InventoryComponent->TryAddingStacksToSpecificSlotWithItemDef(
				ItemDef, CachedStacks, SlotPosition, StacksToAddOverride);

			UpdateDraggedItem(AddingStacksResult, CachedStacks, Controller);
		}
	}
}

void UObsidianItemManagerComponent::ServerTakeoutFromInventoryItem_Implementation(const FIntPoint& SlotPosition,
	const int32 StacksToTake)
{
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* ItemInstance = InventoryComponent->GetItemInstanceAtLocation(SlotPosition);
	if(ItemInstance == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("ItemInstance is null in [%hs]"), __FUNCTION__);
		return;
	}

	const FObsidianItemOperationResult Result = InventoryComponent->TakeOutFromItemInstance(ItemInstance, StacksToTake);
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

	if(ItemDef && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(ItemDef);
	}
}

void UObsidianItemManagerComponent::ServerReplaceItemAtInventorySlot_Implementation(
	const FIntPoint& ClickedItemPosition, const FIntPoint& ClickedGridPosition)
{
	const AController* Controller = Cast<AController>(GetOwner());
	if (Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if (InventoryComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	if (InventoryComponent->CanOwnerModifyInventoryState() == false)
	{
		return;
	}
	
	const FDraggedItem CachedDraggedItem = DraggedItem;
	DraggedItem.Clear();
	StopDraggingItem(Controller);
	
	ServerGrabInventoryItemToCursor(ClickedItemPosition);

	bool bSuccess = false;
	if (UObsidianInventoryItemInstance* Instance = CachedDraggedItem.Instance)
	{
		bSuccess = InventoryComponent->AddItemInstanceToSpecificSlot(Instance, ClickedGridPosition);
	}
	else if (const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = CachedDraggedItem.ItemDef)
	{
		bSuccess = InventoryComponent->AddItemDefinitionToSpecifiedSlot(ItemDef, ClickedGridPosition,
			CachedDraggedItem.GeneratedData);
	}
	
	if (bSuccess == false)
	{
		ServerAddItemToInventoryAtSlot(ClickedItemPosition, false);
		DraggedItem = CachedDraggedItem;
		StartDraggingItem(Controller);
	}
}

void UObsidianItemManagerComponent::ServerGrabDroppableItemToCursor_Implementation(AObsidianDroppableItem* ItemToPickup)
{
	if(ItemToPickup == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("ItemToPickup is null in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	if (VerifyPickupRange(ItemToPickup) == false)
	{
		UE_LOG(LogItemManager, Error, TEXT("[%hs]: Item is too far to be picked up!"), __FUNCTION__);
		return;
	}

	// if(HandlePickUpIfItemOutOfRange(ItemToPickup, EObsidianItemPickUpType::PickUpToDrag))
	// {
	// 	return;
	// }
	
	const FObsidianPickupTemplate Template = ItemToPickup->GetPickupTemplateFromPickupContent();
	if(Template.IsValid()) // We are grabbing Item Template
	{
		DraggedItem = FDraggedItem(Template.ItemDef, Template.ItemGeneratedData);
		ItemToPickup->UpdateDroppedItemStacks(0);
		
		StartDraggingItem(Controller);
		return;
	}

	const FObsidianPickupInstance Instance = ItemToPickup->GetPickupInstanceFromPickupContent();
	if(Instance.IsValid()) // We are grabbing Item Instance
	{
		DraggedItem = FDraggedItem(Instance.Item);
		ItemToPickup->UpdateDroppedItemStacks(0);
		
		StartDraggingItem(Controller);
		return;
	}

	checkf(false, TEXT("Provided ItemToPickup has no Instance nor Template to pick up,"
					" this is bad and should not happen."))
}

void UObsidianItemManagerComponent::ServerGrabInventoryItemToCursor_Implementation(const FIntPoint& SlotPosition)
{
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = InventoryComponent->GetItemInstanceAtLocation(SlotPosition);
	if(InstanceToGrab == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("InstanceToGrab is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	if(InventoryComponent->RemoveItemInstance(InstanceToGrab) == false)
	{
		return;
	}
	
	DraggedItem = FDraggedItem(InstanceToGrab);

	StartDraggingItem(Controller);
}

void UObsidianItemManagerComponent::ServerPickupItem_Implementation(AObsidianDroppableItem* ItemToPickup)
{
	if(ItemToPickup == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("ItemToPickup is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("Controller is null in [%hs]"), __FUNCTION__);
		return;
	}

	if (VerifyPickupRange(ItemToPickup) == false)
	{
		UE_LOG(LogItemManager, Error, TEXT("[%hs]: Item is too far to be picked up!"), __FUNCTION__);
		return;
	}
	
	// if(HandlePickUpIfItemOutOfRange(ItemToPickup, EObsidianItemPickUpType::AutomaticPickUp))
	// {
	// 	return;
	// }
	
	const FObsidianPickupTemplate Template = ItemToPickup->GetPickupTemplateFromPickupContent();
	if(Template.IsValid())
	{
		const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = Template.ItemDef;
		if(ItemDef == nullptr)
		{
			UE_LOG(LogItemManager, Error, TEXT("ItemDef is null in [%hs]"), __FUNCTION__);
			return;
		}

		if(const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject())
		{
			if(DefaultObject->IsEquippable())
			{
				UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
				if(EquipmentComponent == nullptr)
				{
					UE_LOG(LogItemManager, Error, TEXT("OwnerEquipmentComponent is null in [%hs]"),
						__FUNCTION__);
					return;
				}
			
				if(EquipmentComponent->AutomaticallyEquipItem(ItemDef, Template.ItemGeneratedData))
				{
					ItemToPickup->DestroyDroppedItem();
					return;
				}
			}
		}

		UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
		if(InventoryComponent == nullptr)
		{
			UE_LOG(LogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
			return;
		}
		
		if(const FObsidianItemOperationResult& Result = InventoryComponent->AddItemDefinition(ItemDef,
			Template.ItemGeneratedData))
		{
			ItemToPickup->UpdateDroppedItemStacks(Result.StacksLeft);
		}
		
		return;
	}

	const FObsidianPickupInstance Instance = ItemToPickup->GetPickupInstanceFromPickupContent();
	if(Instance.IsValid())
	{
		UObsidianInventoryItemInstance* ItemInstance = Instance.Item;
		if(ItemInstance == nullptr)
		{
			UE_LOG(LogItemManager, Error, TEXT("ItemInstance is null in [%hs]"), __FUNCTION__);
			return;
		}

		if(ItemInstance->IsItemEquippable())
		{
			UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
			if(EquipmentComponent == nullptr)
			{
				UE_LOG(LogItemManager, Error, TEXT("OwnerEquipmentComponent is null in [%hs]"), __FUNCTION__);
				return;
			}
			
			if(EquipmentComponent->AutomaticallyEquipItem(ItemInstance))
			{
				ItemToPickup->DestroyDroppedItem();
				return;
			}
		}

		UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
		if(InventoryComponent == nullptr)
		{
			UE_LOG(LogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
			return;
		}


		const int32 CurrentStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		const FObsidianItemOperationResult Result = InventoryComponent->AddItemInstance(ItemInstance);
		if(CurrentStacks != Result.StacksLeft)
		{
			ItemToPickup->UpdateDroppedItemStacks(Result.StacksLeft);
		}
		return;
	}
}

void UObsidianItemManagerComponent::ServerTransferItemToPlayerStash_Implementation(
	const FIntPoint& FromInventoryPosition, const FGameplayTag& ToStashTab)
{
	const AController* Controller = Cast<AController>(GetOwner());
	if (Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if (PlayerStashComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerPlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if (InventoryComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = InventoryComponent->GetItemInstanceAtLocation(FromInventoryPosition);
	if (InstanceToGrab == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("InstanceToGrab is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	if (PlayerStashComponent->CanFitInstanceInStashTab(InstanceToGrab->GetItemGridSpan(),
		InstanceToGrab->GetItemCategoryTag(), InstanceToGrab->GetItemBaseTypeTag(), ToStashTab))
	{
		if (InventoryComponent->RemoveItemInstance(InstanceToGrab))
		{
			PlayerStashComponent->AddItemInstance(InstanceToGrab, ToStashTab);
		}
	}
}

void UObsidianItemManagerComponent::ServerEquipItemAtSlot_Implementation(const FGameplayTag& SlotTag)
{
	if(DraggedItem.IsEmpty())
	{
		UE_LOG(LogItemManager, Error, TEXT("Tried to add Inventory Item to the Inventory at specific slot"
									 " but the Dragged Item is Empty in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
	if(EquipmentComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerEquipmentComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	if(UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		if(EquipmentComponent->EquipItemToSpecificSlot(Instance, SlotTag))
		{
			DraggedItem.Clear();
			StopDraggingItem(Controller);
		}
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		if(EquipmentComponent->EquipItemToSpecificSlot(ItemDef, SlotTag, DraggedItem.GeneratedData))
		{
			DraggedItem.Clear();
			StopDraggingItem(Controller);
		}
	}
}

void UObsidianItemManagerComponent::ServerGrabEquippedItemToCursor_Implementation(const FGameplayTag& SlotTag)
{
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
	if(EquipmentComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = EquipmentComponent->GetEquippedInstanceAtSlot(SlotTag);
	if(InstanceToGrab == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("InstanceToGrab is null in [%hs]"), __FUNCTION__);
		return;
	}

	EquipmentComponent->UnequipItem(InstanceToGrab);
	
	DraggedItem = FDraggedItem(InstanceToGrab);

	StartDraggingItem(Controller);
}

void UObsidianItemManagerComponent::ServerReplaceItemAtEquipmentSlot_Implementation(const FGameplayTag& SlotTag,
	const FGameplayTag& EquipSlotTagOverride)
{
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
	if(EquipmentComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	if(EquipmentComponent->CanOwnerModifyEquipmentState() == false)
	{
		return;
	}

	const FDraggedItem CachedDraggedItem = DraggedItem;		
	DraggedItem.Clear();
	StopDraggingItem(Controller);
	
	ServerGrabEquippedItemToCursor(SlotTag);

	bool bSuccess = false;
	if(UObsidianInventoryItemInstance* Instance = CachedDraggedItem.Instance)
	{
		bSuccess = EquipmentComponent->ReplaceItemAtSpecificSlot(Instance, SlotTag, EquipSlotTagOverride);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = CachedDraggedItem.ItemDef)
	{
		bSuccess = EquipmentComponent->ReplaceItemAtSpecificSlot(ItemDef, SlotTag, CachedDraggedItem.GeneratedData,
			EquipSlotTagOverride);
	}
	
	if(bSuccess == false)
	{
		ServerEquipItemAtSlot(SlotTag);
		DraggedItem = CachedDraggedItem;
		StartDraggingItem(Controller);
	}
}

void UObsidianItemManagerComponent::ServerWeaponSwap_Implementation()
{
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
	if(EquipmentComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	EquipmentComponent->WeaponSwap();
}

void UObsidianItemManagerComponent::ServerAddItemToStashTabAtSlot_Implementation(
	const FObsidianItemPosition& AtPosition, const bool bShiftDown)
{
	if(DraggedItem.IsEmpty())
	{
		UE_LOG(LogItemManager, Error, TEXT("Tried to add Inventory Item to the Inventory at specific slot"
									 " but the Dragged Item is Empty in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if(PlayerStashComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerPlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const int32 StacksToAddOverride = bShiftDown ? 1 : -1;
	
	if(UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		const int32 CurrentStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		const FObsidianItemOperationResult Result = PlayerStashComponent->AddItemInstanceToSpecificSlot(Instance,
			AtPosition, StacksToAddOverride);
		
		UpdateDraggedItem(Result, CurrentStackCount, Controller);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		const int32 CachedStacks = DraggedItem.GeneratedData.AvailableStackCount;
		const FObsidianItemOperationResult Result = PlayerStashComponent->AddItemDefinitionToSpecifiedSlot(ItemDef,
			AtPosition, DraggedItem.GeneratedData, StacksToAddOverride);

		UpdateDraggedItem(Result, CachedStacks, Controller);
	}
}

void UObsidianItemManagerComponent::ServerAddStacksFromDraggedItemToStashedItemAtSlot_Implementation(
	const FObsidianItemPosition& AtPosition, const int32 StacksToAddOverride)
{
	if(DraggedItem.IsEmpty())
	{
		UE_LOG(LogItemManager, Error, TEXT("Tried to add Stacks from Dragged Item,"
									 " but Dragged Item is empty [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if(PlayerStashComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerPlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianInventoryItemInstance* Instance = DraggedItem.Instance;
	if(Instance && Instance->IsStackable())
	{
		const int32 CurrentStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		const FObsidianAddingStacksResult AddingStacksResult = PlayerStashComponent->TryAddingStacksToSpecificSlotWithInstance(
			Instance, AtPosition, StacksToAddOverride);
		
		UpdateDraggedItem(AddingStacksResult, CurrentStackCount, Controller);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
		if(DefaultObject && DefaultObject->IsStackable())
		{
			const int32 CurrentStackCount = DraggedItem.GeneratedData.AvailableStackCount;
			const FObsidianAddingStacksResult AddingStacksResult = PlayerStashComponent->TryAddingStacksToSpecificSlotWithItemDef(
				ItemDef, CurrentStackCount, AtPosition, StacksToAddOverride);
			
			UpdateDraggedItem(AddingStacksResult, CurrentStackCount, Controller);
		}
	}
}
									
void UObsidianItemManagerComponent::ServerGrabStashedItemToCursor_Implementation(
	const FObsidianItemPosition& FromPosition)
{
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if(PlayerStashComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerPlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = PlayerStashComponent->GetItemInstanceFromTabAtPosition(FromPosition);
	if(InstanceToGrab == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("InstanceToGrab is null in [%hs]"), __FUNCTION__);
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
	const FObsidianItemPosition& FromStashPosition)
{
	const AController* Controller = Cast<AController>(GetOwner());
	if (Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if (PlayerStashComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerPlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if (InventoryComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerInventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = PlayerStashComponent->GetItemInstanceFromTabAtPosition(FromStashPosition);
	if (InstanceToGrab == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("InstanceToGrab is null in [%hs]"), __FUNCTION__);
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
	const FObsidianItemPosition& AtStashPosition)
{
	const AController* Controller = Cast<AController>(GetOwner());
	if (Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if (PlayerStashComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerPlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	if (PlayerStashComponent->CanOwnerModifyPlayerStashState() == false)
	{
		return;
	}
	
	const FDraggedItem CachedDraggedItem = DraggedItem;
	DraggedItem.Clear();
	StopDraggingItem(Controller);
	
	ServerGrabStashedItemToCursor(AtStashPosition);

	bool bSuccess = false;
	if (UObsidianInventoryItemInstance* Instance = CachedDraggedItem.Instance)
	{
		bSuccess = PlayerStashComponent->AddItemInstanceToSpecificSlot(Instance, AtStashPosition);
	}
	else if (const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = CachedDraggedItem.ItemDef)
	{
		bSuccess = PlayerStashComponent->AddItemDefinitionToSpecifiedSlot(ItemDef, AtStashPosition,
			CachedDraggedItem.GeneratedData);
	}
	
	if (bSuccess == false)
	{
		ServerAddItemToStashTabAtSlot(AtStashPosition, false);
		DraggedItem = CachedDraggedItem;
		StartDraggingItem(Controller);
	}
}

void UObsidianItemManagerComponent::ServerTakeoutFromStashedItem_Implementation(
	const FObsidianItemPosition& AtStashPosition, const int32 StacksToTake)
{
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if(PlayerStashComponent == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerPlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* ItemInstance = PlayerStashComponent->GetItemInstanceFromTabAtPosition(AtStashPosition);
	if(ItemInstance == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("ItemInstance is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const FObsidianItemOperationResult Result = PlayerStashComponent->TakeOutFromItemInstance(ItemInstance, StacksToTake);
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

	if(ItemDef && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(ItemDef);
	}
}

bool UObsidianItemManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	UObsidianInventoryItemInstance* Instance = DraggedItem.Instance;
	if(Instance && IsValid(Instance))
	{
		WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
	}
	
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef;
	if(ItemDef && IsValid(ItemDef))
	{
		WroteSomething |= Channel->ReplicateSubobject(ItemDef, *Bunch, *RepFlags);
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

		const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef;
		if(IsValid(ItemDef))
		{
			AddReplicatedSubObject(ItemDef);
		}
	}
}

void UObsidianItemManagerComponent::OnRep_DraggedItem(const FDraggedItem& OldDraggedItem)
{
	if(DraggedItem.IsEmpty() && bDraggingItem) // We cleared Dragged Item, so we should no longer drag it
	{
		const AController* Controller = Cast<AController>(GetOwner());
		if(Controller == nullptr)
		{
			UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
			return;
		}
		StopDraggingItem(Controller);
	}
	else if(!DraggedItem.IsEmpty() && !bDraggingItem || DraggedItemWasReplaced(OldDraggedItem))  // We got new Item to drag
	{
		const AController* Controller = Cast<AController>(GetOwner());
		if(Controller == nullptr)
		{
			UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
			return;
		}
		StartDraggingItem(Controller);
	}
	else if(DraggedItem.GeneratedData.AvailableStackCount > 0) // We are dragging an item but the stacks changed. //TODO(intrxx) Why just "Stacks > 0"?
	{
		if(DraggedItemWidget)
		{
			DraggedItemWidget->UpdateStackCount(DraggedItem.GeneratedData.AvailableStackCount);
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

void UObsidianItemManagerComponent::StartDraggingItem(const AController* Controller)
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}
	
	if(Controller && !Controller->IsLocalController())
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

void UObsidianItemManagerComponent::StopDraggingItem(const AController* Controller)
{
	if(Controller && !Controller->IsLocalController())
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

void UObsidianItemManagerComponent::UpdateDraggedItem(const FObsidianItemOperationResult& OperationResult,
	const int32 CachedNumberOfStack, const AController* ForController)
{
	if(CachedNumberOfStack != OperationResult.StacksLeft)
	{
		if(OperationResult.bActionSuccessful)
		{
			DraggedItem.Clear();
			StopDraggingItem(ForController);
			return;
		}
		UpdateStacksOnDraggedItemWidget(OperationResult.StacksLeft);
		DraggedItem.GeneratedData.AvailableStackCount = OperationResult.StacksLeft;
	}
}

void UObsidianItemManagerComponent::UpdateDraggedItem(const FObsidianAddingStacksResult& OperationResult,
	const int32 CachedNumberOfStack, const AController* ForController)
{
	if(CachedNumberOfStack != OperationResult.StacksLeft)
	{
		if(OperationResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded)
		{
			DraggedItem.Clear();
			StopDraggingItem(ForController);
		}
		else if(OperationResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded)
		{
			UpdateStacksOnDraggedItemWidget(OperationResult.StacksLeft);
			DraggedItem.GeneratedData.AvailableStackCount = OperationResult.StacksLeft;
		}
	}
}

bool UObsidianItemManagerComponent::DraggedItemWasReplaced(const FDraggedItem& OldDraggedItem) const
{
	if(OldDraggedItem.Instance && OldDraggedItem.Instance != DraggedItem.Instance)
	{
		return true;
	}

	if(OldDraggedItem.ItemDef && DraggedItem.Instance)
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

bool UObsidianItemManagerComponent::VerifyPickupRange(const AObsidianDroppableItem* ItemToPickUp)
{
	//TODO(intrxx) this is hardcoded DefaultInteractionRadius + AutoRunAcceptanceRadius from Input Manager get this somehow
	constexpr float DefaultInteractionRadius = 200.0f;
	constexpr float AutoRunAcceptanceRadius = 30.0f;
	
	const APlayerController* PC = Cast<APlayerController>(GetOwner());
	if(PC == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("ObsidianPC is null in [%hs]"), __FUNCTION__);
		return false;
	}

	const ACharacter* OwnerCharacter = PC->GetCharacter();
	if (OwnerCharacter == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwnerCharacter is null in [%hs]"), __FUNCTION__);
		return false;
	}

	const FVector ItemLocation = ItemToPickUp->GetActorLocation();
	const FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	
	const float DistanceToItem = FVector::Dist2D(
		FVector(OwnerLocation.X, OwnerLocation.Y, 0.0f),
		FVector(ItemLocation.X, ItemLocation.Y, 0.0f)); 
	if (DistanceToItem > DefaultInteractionRadius + AutoRunAcceptanceRadius + 10.0f) 
	{
		return false;
	}
	return true;
}

void UObsidianItemManagerComponent::ServerHandleDroppingItem_Implementation()
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}

	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	ACharacter* OwningCharacter = Controller->GetCharacter();
	if (OwningCharacter == nullptr)
	{
		UE_LOG(LogItemManager, Error, TEXT("OwningCharacter is null in [%hs]"), __FUNCTION__);
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
		UE_LOG(LogItemManager, Error, TEXT("NavigationSystem is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	//TODO(intrxx) Bias towards Actors forward Vector
	FNavLocation RandomPointLocation;
	NavigationSystem->GetRandomPointInNavigableRadius(OwnerLocation, DropRadius, RandomPointLocation);
	
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

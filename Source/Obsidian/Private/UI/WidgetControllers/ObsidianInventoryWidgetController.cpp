// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"

// ~ Core
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"

// ~ Project
#include "UI/Inventory/ObsidianItemDescriptionBase.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "CharacterComponents/ObsidianHeroComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Core/ObsidianGameplayStatics.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/ObsidianHUD.h"
#include "InventoryItems/Equipment/ObsidianEquipmentList.h"
#include "UI/Inventory/ObsidianDraggedItem.h"
#include "UI/Inventory/ObsidianItem.h"
#include "UI/Inventory/ObsidianSlotBlockadeItem.h"
#include "UI/Inventory/SubWidgets/ObsidianItemSlot_Equipment.h"
#include "UI/Inventory/SubWidgets/ObsidianUnstackSlider.h"
#include "UI/MainOverlay/ObsidianMainOverlay.h"

void UObsidianInventoryWidgetController::OnWidgetControllerSetupCompleted()
{
	check(InventoryComponent);
	check(EquipmentComponent);
	check(ObsidianPlayerController);
	
	const AActor* OwningActor = Cast<AActor>(ObsidianPlayerController->GetPawn());
	check(OwningActor);
	
	OwnerHeroComponent = UObsidianHeroComponent::FindHeroComponent(OwningActor);
	check(OwnerHeroComponent);
	OwnerHeroComponent->OnStopUsingItemDelegate.AddUObject(this, &ThisClass::ClearUsableUIContext);

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwningActor->GetWorld());
	MessageSubsystem.RegisterListener(ObsidianGameplayTags::Message_Inventory_Changed, this, &ThisClass::OnInventoryStateChanged);
	MessageSubsystem.RegisterListener(ObsidianGameplayTags::Message_Equipment_Changed, this, &ThisClass::OnEquipmentStateChanged);

	
}

void UObsidianInventoryWidgetController::OnInventoryStateChanged(FGameplayTag Channel, const FObsidianInventoryChangeMessage& InventoryChangeMessage)
{
	if(InventoryComponent != InventoryChangeMessage.InventoryOwner) // Fixes a bug when Items appear in Server's Inventory (Listen Server Character) after picked up by client.
	{
		return;
	}
	
	const UObsidianInventoryItemInstance* Instance = InventoryChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Instance is invalid in UObsidianInventoryWidgetController::OnInventoryStateChanged."));
		return;
	}

	if(InventoryChangeMessage.ChangeType == EObsidianInventoryChangeType::ICT_GeneralItemChanged)
	{
		HandleHoveringOverInventoryItem(InventoryChangeMessage.GridItemPosition);
		if(UObsidianItem* CorrespondingItemWidget = GetItemWidgetAtInventoryGridSlot(InventoryChangeMessage.GridItemPosition))
		{
			CorrespondingItemWidget->ResetHighlight();
			CachedItemsMatchingUsableContext.Remove(CorrespondingItemWidget);
		}
	}
	else if(InventoryChangeMessage.ChangeType == EObsidianInventoryChangeType::ICT_ItemAdded)
	{
		UE_LOG(LogInventory, Display, TEXT("Added item: [%s]"), *Instance->GetItemDisplayName().ToString());
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemImage = Instance->GetItemImage();
		ItemWidgetData.DesiredPosition = InventoryChangeMessage.GridItemPosition;
		ItemWidgetData.GridSpan = Instance->GetItemGridSpan();
		ItemWidgetData.StackCount = Instance->IsStackable() ? InventoryChangeMessage.NewCount : 0;
		ItemWidgetData.bUsable = Instance->IsItemUsable();
		ItemWidgetData.ItemSlotPadding = Instance->GetItemSlotPadding();
		
		OnItemAddedDelegate.Broadcast(ItemWidgetData);
	}
	else if(InventoryChangeMessage.ChangeType == EObsidianInventoryChangeType::ICT_ItemRemoved)
	{
		UE_LOG(LogInventory, Display, TEXT("Removed item: [%s]"), *Instance->GetItemDisplayName().ToString());
		RemoveInventoryItemWidget(InventoryChangeMessage.GridItemPosition);
		
		if(OwnerHeroComponent->IsUsingItem())
		{
			if(InventoryChangeMessage.ItemInstance == OwnerHeroComponent->GetUsingItem())
			{
				OwnerHeroComponent->SetUsingItem(false); //TODO This probably shouldn't be here, its widget controller
			}
		}
	}
	else if (InventoryChangeMessage.ChangeType == EObsidianInventoryChangeType::ICT_ItemStacksChanged)
	{
		UE_LOG(LogInventory, Display, TEXT("Changed item: [%s]"), *Instance->GetItemDisplayName().ToString());
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.DesiredPosition = InventoryChangeMessage.GridItemPosition;
		ItemWidgetData.StackCount = Instance->IsStackable() ? InventoryChangeMessage.NewCount : 0;
		
		OnItemChangedDelegate.Broadcast(ItemWidgetData);
	}
}

void UObsidianInventoryWidgetController::OnEquipmentStateChanged(FGameplayTag Channel, const FObsidianEquipmentChangeMessage& EquipmentChangeMessage)
{
	if(EquipmentComponent != EquipmentChangeMessage.EquipmentOwner) // Fixes a bug when Items appear in Server's Inventory (Listen Server Character) after picked up by client.
	{
		return;
	}
	
	const UObsidianInventoryItemInstance* Instance = EquipmentChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Instance is invalid in UObsidianInventoryWidgetController::OnEquipmentStateChanged."));
		return;
	}

	if(EquipmentChangeMessage.ChangeType == EObsidianEquipmentChangeType::ECT_ItemEquipped)
	{
		UE_LOG(LogInventory, Display, TEXT("Equipped item: [%s]"), *Instance->GetItemDisplayName().ToString());

		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemImage = Instance->GetItemImage();
		ItemWidgetData.DesiredSlot = EquipmentChangeMessage.SlotTag;
		ItemWidgetData.GridSpan = Instance->GetItemGridSpan();
		ItemWidgetData.bDoesBlockSisterSlot = Instance->DoesItemNeedsTwoSlots();
		ItemWidgetData.ItemCategory = Instance->GetItemCategoryTag();
		ItemWidgetData.ItemSlotPadding = Instance->GetItemSlotPadding();
		
		OnItemEquippedDelegate.Broadcast(ItemWidgetData);
	}
	else if(EquipmentChangeMessage.ChangeType == EObsidianEquipmentChangeType::ECT_ItemUnequipped)
	{
		UE_LOG(LogInventory, Display, TEXT("Unequipped item: [%s]"), *Instance->GetItemDisplayName().ToString());

		const FGameplayTag SlotTagToClear = EquipmentChangeMessage.SlotTagToClear;
		if(SlotTagToClear.IsValid())
		{
			RemoveEquipmentItemWidget(SlotTagToClear);
		}

		if(Instance->DoesItemNeedsTwoSlots())
		{
			RemoveBlockedSlotItemWidget(SlotTagToClear);
		}
	}
	else if(EquipmentChangeMessage.ChangeType == EObsidianEquipmentChangeType::ECT_ItemSwapped)
	{
		UE_LOG(LogInventory, Display, TEXT("Swapped item: [%s]"), *Instance->GetItemDisplayName().ToString());
		
		const FGameplayTag SlotTagToClear = EquipmentChangeMessage.SlotTagToClear;
		if(SlotTagToClear.IsValid())
		{
			RemoveEquipmentItemWidget(SlotTagToClear);
		}

		if(Instance->DoesItemNeedsTwoSlots())
		{
			const FGameplayTag EquipmentTag = SlotTagToClear == FGameplayTag::EmptyTag ? UObsidianGameplayStatics::GetOpposedEuipmentTagForTag(EquipmentChangeMessage.SlotTag) : EquipmentChangeMessage.SlotTagToClear;
			RemoveBlockedSlotItemWidget(EquipmentTag);
		}
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemImage = Instance->GetItemImage();
		ItemWidgetData.DesiredSlot = EquipmentChangeMessage.SlotTag;
		ItemWidgetData.GridSpan = Instance->GetItemGridSpan();
		ItemWidgetData.bSwappedWithAnotherItem = EquipmentChangeMessage.SlotTagToClear == FGameplayTag::EmptyTag;
		ItemWidgetData.bDoesBlockSisterSlot = Instance->DoesItemNeedsTwoSlots();
		ItemWidgetData.ItemCategory = Instance->GetItemCategoryTag();
		ItemWidgetData.ItemSlotPadding = Instance->GetItemSlotPadding();
		
		OnItemEquippedDelegate.Broadcast(ItemWidgetData);
	}
}

int32 UObsidianInventoryWidgetController::GetInventoryGridWidth() const
{
	if(InventoryComponent)
	{
		return InventoryComponent->GetInventoryGridWidth();
	}
	
	UE_LOG(LogInventory, Error, TEXT("Trying to return Grid Width but Inventory Component is invalid."))
	return 0;
}

int32 UObsidianInventoryWidgetController::GetInventoryGridHeight() const
{
	if(InventoryComponent)
	{
		return InventoryComponent->GetInventoryGridHeight();
	}
	
	UE_LOG(LogInventory, Error, TEXT("Trying to return Grid Height but Inventory Component is invalid."))
	return 0;
}

void UObsidianInventoryWidgetController::OnInventoryOpen()
{
	TArray<UObsidianInventoryItemInstance*> InventoryItems = InventoryComponent->GetAllItems();
	AddedItemWidgetMap.Empty(InventoryItems.Num());
	
	for(const UObsidianInventoryItemInstance* Item : InventoryItems)
	{
		ensure(Item);
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemImage = Item->GetItemImage();
		ItemWidgetData.GridSpan = Item->GetItemGridSpan();
		ItemWidgetData.DesiredPosition = Item->GetItemCurrentGridLocation();
		ItemWidgetData.StackCount = Item->IsStackable() ? Item->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current) : 0;
		ItemWidgetData.bUsable = Item->IsItemUsable();
		ItemWidgetData.ItemSlotPadding = Item->GetItemSlotPadding();

		OnItemAddedDelegate.Broadcast(ItemWidgetData);
	}

	TArray<UObsidianInventoryItemInstance*> EquippedItems = EquipmentComponent->GetAllEquippedItems();
	EquippedItemWidgetMap.Empty(EquippedItems.Num());
	BlockedSlotsWidgetMap.Empty();

	for(const UObsidianInventoryItemInstance* Item : EquippedItems)
	{
		ensure(Item);
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemImage = Item->GetItemImage();
		ItemWidgetData.GridSpan = Item->GetItemGridSpan();
		ItemWidgetData.DesiredSlot = Item->GetItemCurrentEquipmentSlot();
		ItemWidgetData.bDoesBlockSisterSlot = Item->DoesItemNeedsTwoSlots();
		ItemWidgetData.ItemSlotPadding = Item->GetItemSlotPadding();
		
		OnItemEquippedDelegate.Broadcast(ItemWidgetData);
	}
}

void UObsidianInventoryWidgetController::RequestAddingItemToInventory(const FIntPoint& ToGridSlot, const bool bShiftDown)
{
	check(OwnerHeroComponent);

	if(OwnerHeroComponent->IsUsingItem())
	{
		StopUsingItem();
		return;
	}
	
	if(OwnerHeroComponent->IsDraggingAnItem() == false)
	{
		return;
	}
	OwnerHeroComponent->ServerAddItemToInventoryAtSlot(ToGridSlot, bShiftDown);
}

void UObsidianInventoryWidgetController::RequestEquippingItem(const FGameplayTag& SlotTag)
{
	check(OwnerHeroComponent);

	if(OwnerHeroComponent->IsUsingItem())
	{
		StopUsingItem();
		return;
	}
	
	if(OwnerHeroComponent->IsDraggingAnItem() == false)
	{
		return;
	}
	OwnerHeroComponent->ServerEquipItemAtSlot(SlotTag);
}

void UObsidianInventoryWidgetController::HandleRightClickingOnInventoryItem(const FIntPoint& AtGridSlot, UObsidianItem* ItemWidget)
{
	check(OwnerHeroComponent);
	check(InventoryComponent);

	if(OwnerHeroComponent->IsDraggingAnItem())
	{
		return;
	}

	if(InventoryComponent->CanOwnerModifyInventoryState() == false)
	{
		return;
	}
	
	UObsidianInventoryItemInstance* UsingInstance = InventoryComponent->GetItemInstanceAtLocation(AtGridSlot);
	if(UsingInstance->IsItemUsable() == false)
	{
		return;
	}
	
	OwnerHeroComponent->SetUsingItem(true, ItemWidget, UsingInstance);

	TArray<UObsidianInventoryItemInstance*> AllItems;
	AllItems.Append(InventoryComponent->GetAllItems());
	AllItems.Append(EquipmentComponent->GetAllEquippedItems());
	//TODO Add Stash when implemented
	
	const FObsidianItemsMatchingUsableContext MatchingUsableContext = UsingInstance->FireItemUseUIContext(AllItems);
	for(const FIntPoint& GridLocation : MatchingUsableContext.InventoryItemsMatchingContext)
	{
		if(UObsidianItem* Item = GetItemWidgetAtInventoryGridSlot(GridLocation))
		{
			Item->HighlightItem();
			CachedItemsMatchingUsableContext.Add(Item);
		}
	}
	for(const FGameplayTag& SlotTag : MatchingUsableContext.EquipmentItemsMatchingContext)
	{
		if(UObsidianItem* Item = GetItemWidgetAtEquipmentSlot(SlotTag))
		{
			Item->HighlightItem();
			CachedItemsMatchingUsableContext.Add(Item);
		}
	}
	for(const FIntPoint& StashGridLocation : MatchingUsableContext.StashItemsMatchingContext)
	{
		
	}
}

void UObsidianInventoryWidgetController::HandleLeftClickingOnInventoryItem(const FIntPoint& AtGridSlot)
{
	check(InventoryComponent);
	check(OwnerHeroComponent);
	check(DraggedItemWidgetClass);

	if(InventoryComponent->CanOwnerModifyInventoryState() == false)
	{
		return;
	}
	
	RemoveItemUIElements();

	if(OwnerHeroComponent->IsUsingItem())
	{
		OwnerHeroComponent->UseItem(AtGridSlot, false);
		return;
	}
	
	if(OwnerHeroComponent->IsDraggingAnItem()) // If we carry an item, try to add it to this item or replace it with it.
	{
		const UObsidianInventoryItemInstance* InstanceToAddTo = InventoryComponent->GetItemInstanceAtLocation(AtGridSlot);
		if(InstanceToAddTo == nullptr)
		{
			UE_LOG(LogInventory, Error, TEXT("Item Instance at pressed Location is invalid in UObsidianInventoryWidgetController::HandleLeftClickingOnAnItem."));
			return;
		}
		
		 const FDraggedItem DraggedItem = OwnerHeroComponent->GetDraggedItem();
		 if(UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance) // We carry item instance.
		 {
		 	if(DraggedInstance->IsStackable())
		 	{
		 		if(UObsidianInventoryComponent::IsTheSameItem(DraggedInstance, InstanceToAddTo))
		 		{
		 			OwnerHeroComponent->ServerAddStacksFromDraggedItemToItemAtSlot(AtGridSlot);
		 			return;
		 		}
		 	}
			
		 	if(InventoryComponent->CanReplaceItemAtSpecificSlotWithInstance(AtGridSlot, DraggedInstance))
		 	{
		 		OwnerHeroComponent->ServerReplaceItemAtInventorySlot(AtGridSlot);
		 	}
		 	return;
		 }
		
		 if(const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef) // We carry item def
		 {
		 	const int32 ItemStackCount = DraggedItem.Stacks;
		 	const UObsidianInventoryItemDefinition* DefaultObject = DraggedItemDef.GetDefaultObject();
		 	if(DefaultObject && DefaultObject->IsStackable())
		 	{
		 		if(UObsidianInventoryComponent::IsTheSameItem(InstanceToAddTo, DraggedItemDef))
		 		{
		 			OwnerHeroComponent->ServerAddStacksFromDraggedItemToItemAtSlot(AtGridSlot);
		 			return;
		 		}
		 	}
			
			 if(InventoryComponent->CanReplaceItemAtSpecificSlotWithDef(AtGridSlot, DraggedItemDef, ItemStackCount))
			 {
			 	OwnerHeroComponent->ServerReplaceItemAtInventorySlot(AtGridSlot);
			 }
			 return;
		}
		return;
	}
	
	OwnerHeroComponent->ServerGrabInventoryItemToCursor(AtGridSlot);
}

void UObsidianInventoryWidgetController::HandleLeftClickingOnInventoryItemWithShiftDown(const FIntPoint& AtGridSlot, const UObsidianItem* ItemWidget)
{
	check(OwnerHeroComponent);

	if(InventoryComponent->CanOwnerModifyInventoryState() == false)
	{
		return;
	}
	
	if(OwnerHeroComponent->IsUsingItem())
	{
		OwnerHeroComponent->UseItem(AtGridSlot, true);
		return;
	}
	
	if(OwnerHeroComponent->IsDraggingAnItem())
	{
		OwnerHeroComponent->ServerAddStacksFromDraggedItemToItemAtSlot(AtGridSlot, 1);
		return;
	}
	
	UObsidianInventoryItemInstance* ItemInstance = InventoryComponent->GetItemInstanceAtLocation(AtGridSlot);
	if(ItemInstance->IsStackable() == false)
	{
		return;
	}
	
	const int32 CurrentItemStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(CurrentItemStacks <= 1)
	{
		return;
	}
	
	RemoveItemUIElements();

	checkf(UnstackSliderClass, TEXT("Tried to create widget without valid widget class in UObsidianInventoryWidgetController::HandleLeftClickingOnAnItemWithShiftDown, fill it in ObsidianInventoryWidgetController instance."));
	ActiveUnstackSlider = CreateWidget<UObsidianUnstackSlider>(ObsidianPlayerController, UnstackSliderClass);
	ActiveUnstackSlider->InitializeUnstackSlider(CurrentItemStacks, AtGridSlot);

	const FVector2D UnstackSliderViewportPosition = CalculateUnstackSliderPosition(ItemWidget);
	ActiveUnstackSlider->SetPositionInViewport(UnstackSliderViewportPosition);
	ActiveUnstackSlider->AddToViewport();
	bUnstackSliderActive = true;
	
	ActiveUnstackSlider->OnAcceptButtonPressedDelegate.AddUObject(this, &ThisClass::HandleTakingOutStacks);
	ActiveUnstackSlider->OnCloseButtonPressedDelegate.AddUObject(this, &ThisClass::RemoveUnstackSlider);
}

void UObsidianInventoryWidgetController::HandleLeftClickingOnEquipmentItem(const FGameplayTag& SlotTag, const FGameplayTag& EquipSlotTagOverride)
{
	const FGameplayTag WeaponSwapSlotTag = FGameplayTag::RequestGameplayTag(TEXT("Equipment.SwapSlot.Weapon"));
	if(SlotTag.MatchesTag(WeaponSwapSlotTag))
	{
		//TODO Cannot left-click on swapped item, add VO?
		return;
	}
	
	check(EquipmentComponent);
	check(OwnerHeroComponent);
	check(DraggedItemWidgetClass);
	
	if(EquipmentComponent->CanOwnerModifyEquipmentState() == false)
	{
		return;
	}
	
	RemoveItemUIElements();

	if(OwnerHeroComponent->IsUsingItem())
	{
		UE_LOG(LogInventory, Error, TEXT("As of now it is imposible to use items onto equipped items, maybe will support it in the future."));
		StopUsingItem();
		return;
	}
	
	if(OwnerHeroComponent->IsDraggingAnItem()) // If we carry an item, try to replace it with it.
	{
		
		 const FDraggedItem DraggedItem = OwnerHeroComponent->GetDraggedItem();
		 if(UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance) // We carry item instance.
		 {
		 	const EObsidianEquipResult EquipmentResult = EquipmentComponent->CanReplaceInstance(DraggedInstance, SlotTag);
		 	if(EquipmentResult == EObsidianEquipResult::CanEquip)
		 	{
		 		OwnerHeroComponent->ServerReplaceItemAtEquipmentSlot(SlotTag, EquipSlotTagOverride);
		 	}
		 	else
		 	{
		 		//TODO Send Client RPC with some voice over passing EquipResult?
#if !UE_BUILD_SHIPPING
		 		UE_LOG(LogEquipment, Warning, TEXT("Item cannot be equipped, reason: [%s]"), *ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipmentResult));
#endif
		 	}
		 	return;
		 }
		
		 if(const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef) // We carry item def
		 {
		 	const EObsidianEquipResult EquipmentResult = EquipmentComponent->CanReplaceTemplate(DraggedItemDef, SlotTag);
		 	if(EquipmentResult == EObsidianEquipResult::CanEquip)
		 	{
		 		OwnerHeroComponent->ServerReplaceItemAtEquipmentSlot(SlotTag, EquipSlotTagOverride);
		 	}
		 	else
		 	{
		 		//TODO Send Client RPC with some voice over passing EquipResult?
#if !UE_BUILD_SHIPPING
			 	UE_LOG(LogEquipment, Warning, TEXT("Item cannot be equipped, reason: [%s]"), *ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipmentResult));
#endif
			 }
			 return;
		}
		return;
	}
	OwnerHeroComponent->ServerGrabEquippedItemToCursor(SlotTag);
}

void UObsidianInventoryWidgetController::HandleHoveringOverInventoryItem(const FIntPoint& AtGridSlot)
{
	const UObsidianItem* ItemWidget = GetItemWidgetAtInventoryGridSlot(AtGridSlot);
	if(ItemWidget == nullptr || !CanShowDescription())
	{
		return;
	}
	
	RemoveItemDescription();
	
	const FObsidianItemStats ItemStats = InventoryComponent->GetItemStatsByInventoryPosition(AtGridSlot);

	checkf(ItemDescriptionClass, TEXT("Tried to create widget without valid widget class in UObsidianInventoryWidgetController::HandleHoveringOverItem, fill it in ObsidianInventoryWidgetController instance."));
	ActiveItemDescription = CreateWidget<UObsidianItemDescriptionBase>(ObsidianPlayerController, ItemDescriptionClass);
	ActiveItemDescription->InitializeWidgetWithItemStats(ItemStats);
	ActiveItemDescription->AddToViewport();
	
	const FVector2D DescriptionViewportPosition = CalculateDescriptionPosition(ItemWidget);
	ActiveItemDescription->SetPositionInViewport(DescriptionViewportPosition);
	bDescriptionActive = true;
}

void UObsidianInventoryWidgetController::HandleHoveringOverInventoryItem(const UObsidianItem* ItemWidget)
{
	if(ItemWidget == nullptr || !CanShowDescription())
	{
		return;
	}
	
	RemoveItemDescription();

	const FIntPoint SlotPosition = ItemWidget->GetInventoryPosition();
	const FObsidianItemStats ItemStats = InventoryComponent->GetItemStatsByInventoryPosition(SlotPosition);

	checkf(ItemDescriptionClass, TEXT("Tried to create widget without valid widget class in UObsidianInventoryWidgetController::HandleHoveringOverItem, fill it in ObsidianInventoryWidgetController instance."));
	ActiveItemDescription = CreateWidget<UObsidianItemDescriptionBase>(ObsidianPlayerController, ItemDescriptionClass);
	ActiveItemDescription->InitializeWidgetWithItemStats(ItemStats);
	ActiveItemDescription->SetAssociatedInventoryLocation(SlotPosition);
	ActiveItemDescription->AddToViewport();
	
	const FVector2D DescriptionViewportPosition = CalculateDescriptionPosition(ItemWidget);
	ActiveItemDescription->SetPositionInViewport(DescriptionViewportPosition);
	bDescriptionActive = true;
}

void UObsidianInventoryWidgetController::HandleHoveringOverEquipmentItem(const UObsidianItem* ItemWidget)
{
	if(ItemWidget == nullptr || !CanShowDescription())
	{
		return;
	}
	
	RemoveItemDescription();

	const FGameplayTag SlotTag = ItemWidget->GetEquipmentSlotTag();
	const FObsidianItemStats ItemStats = EquipmentComponent->GetItemStatsBySlotTag(SlotTag);

	checkf(ItemDescriptionClass, TEXT("Tried to create widget without valid widget class in UObsidianInventoryWidgetController::HandleHoveringOverItem, fill it in ObsidianInventoryWidgetController instance."));
	ActiveItemDescription = CreateWidget<UObsidianItemDescriptionBase>(ObsidianPlayerController, ItemDescriptionClass);
	ActiveItemDescription->InitializeWidgetWithItemStats(ItemStats);
	ActiveItemDescription->SetAssociatedSlotTag(SlotTag);
	ActiveItemDescription->AddToViewport();
	
	const FVector2D DescriptionViewportPosition = CalculateDescriptionPosition(ItemWidget);
	ActiveItemDescription->SetPositionInViewport(DescriptionViewportPosition);
	bDescriptionActive = true;
}

bool UObsidianInventoryWidgetController::CanShowDescription() const
{
	return !bUnstackSliderActive;
}

void UObsidianInventoryWidgetController::HandleUnhoveringItem()
{
	RemoveItemDescription();
}

UObsidianItemDescriptionBase* UObsidianInventoryWidgetController::CreateItemDescriptionForDroppedItem(const UObsidianInventoryItemInstance* Instance)
{
	if(!CanShowDescription())
	{
		return nullptr;
	}

	AObsidianHUD* ObsidianHUD = ObsidianPlayerController->GetObsidianHUD();
	if(ObsidianHUD == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Unable to get ObsidianHUD in UObsidianInventoryWidgetController::CreateItemDescriptionForDroppedItem."));
		return nullptr;
	}

	const UObsidianMainOverlay* MainOverlay = ObsidianHUD->GetMainOverlay();
	if(MainOverlay == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Unable to get ObsidianMainOverlay in UObsidianInventoryWidgetController::CreateItemDescriptionForDroppedItem."));
		return nullptr;
	}

	const FObsidianItemStats ItemStats = InventoryComponent->GetItemStatForInstance(Instance);

	checkf(ItemDescriptionClass, TEXT("Tried to create widget without valid widget class in UObsidianInventoryWidgetController::CreateItemDescriptionForDroppedItem, fill it in ObsidianInventoryWidgetController instance."));
	ActiveItemDescription = CreateWidget<UObsidianItemDescriptionBase>(ObsidianPlayerController, ItemDescriptionClass);
	ActiveItemDescription->InitializeWidgetWithItemStats(ItemStats, true);
	MainOverlay->AddItemDescriptionToOverlay(ActiveItemDescription);

	return ActiveItemDescription;
}

UObsidianItemDescriptionBase* UObsidianInventoryWidgetController::CreateItemDescriptionForDroppedItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 CurrentItemStacks)
{
	if(!CanShowDescription())
	{
		return nullptr;
	}

	AObsidianHUD* ObsidianHUD = ObsidianPlayerController->GetObsidianHUD();
	if(ObsidianHUD == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Unable to get ObsidianHUD in UObsidianInventoryWidgetController::CreateItemDescriptionForDroppedItem."));
		return nullptr;
	}

	const UObsidianMainOverlay* MainOverlay = ObsidianHUD->GetMainOverlay();
	if(MainOverlay == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Unable to get ObsidianMainOverlay in UObsidianInventoryWidgetController::CreateItemDescriptionForDroppedItem."));
		return nullptr;
	}
	
	const FObsidianItemStats ItemStats = InventoryComponent->GetItemStatsForItemDefinition(ItemDef, CurrentItemStacks);

	checkf(ItemDescriptionClass, TEXT("Tried to create widget without valid widget class in UObsidianInventoryWidgetController::CreateItemDescriptionForDroppedItem, fill it in ObsidianInventoryWidgetController instance."));
	ActiveItemDescription = CreateWidget<UObsidianItemDescriptionBase>(ObsidianPlayerController, ItemDescriptionClass);
	ActiveItemDescription->InitializeWidgetWithItemStats(ItemStats, true);
	MainOverlay->AddItemDescriptionToOverlay(ActiveItemDescription);

	return ActiveItemDescription;
}

void UObsidianInventoryWidgetController::HandleTakingOutStacks(const int32 StacksToTake, const FIntPoint& GridSlotPosition)
{
	RemoveUnstackSlider();
	
	if(StacksToTake == 0)
	{
		return;
	}

	const UObsidianInventoryItemInstance* Instance = InventoryComponent->GetItemInstanceAtLocation(GridSlotPosition);
	if(Instance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SlotPosition: [%d, %d]"), GridSlotPosition.X, GridSlotPosition.Y);
		return;
	}

	
	if(Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current) == StacksToTake)
	{
		UE_LOG(LogTemp, Warning, TEXT("SlotPosition: [%d, %d]"), GridSlotPosition.X, GridSlotPosition.Y);
		UE_LOG(LogTemp, Warning, TEXT("Instance at position: [%s]"), *InventoryComponent->GetItemInstanceAtLocation(GridSlotPosition)->GetItemDebugName());
		OwnerHeroComponent->ServerGrabInventoryItemToCursor(GridSlotPosition);
		return;
	}
	OwnerHeroComponent->ServerTakeoutFromItem(GridSlotPosition, StacksToTake);
}

void UObsidianInventoryWidgetController::RemoveItemUIElements()
{
	RemoveUnstackSlider();
	RemoveItemDescription();
}

void UObsidianInventoryWidgetController::RemoveUnstackSlider()
{
	if(bUnstackSliderActive && ActiveUnstackSlider != nullptr)
	{
		if(ActiveUnstackSlider->OnAcceptButtonPressedDelegate.IsBound())
		{
			ActiveUnstackSlider->OnAcceptButtonPressedDelegate.Clear();
		}
		if(ActiveUnstackSlider->OnCloseButtonPressedDelegate.IsBound())
		{
			ActiveUnstackSlider->OnCloseButtonPressedDelegate.Clear();
		}
		
		ActiveUnstackSlider->DestroyUnstackSlider();
		ActiveUnstackSlider = nullptr;
		bUnstackSliderActive = false;
	}
}

void UObsidianInventoryWidgetController::RemoveItemDescription()
{
	if(bDescriptionActive && ActiveItemDescription)
	{
		ActiveItemDescription->DestroyDescriptionWidget();
		ActiveItemDescription = nullptr;
		bDescriptionActive = false;
	}
}

void UObsidianInventoryWidgetController::StopUsingItem()
{
	OwnerHeroComponent->SetUsingItem(false);
}

void UObsidianInventoryWidgetController::ClearUsableUIContext()
{
	if(CachedItemsMatchingUsableContext.IsEmpty())
	{
		return;
	}

	for(UObsidianItem* AffectedItem : CachedItemsMatchingUsableContext)
	{
		if(AffectedItem)
		{
			AffectedItem->ResetHighlight();
		}
	}
}

bool UObsidianInventoryWidgetController::IsDraggingAnItem() const
{
	if(OwnerHeroComponent)
	{
		return OwnerHeroComponent->IsDraggingAnItem();
	}
	return false;
}

bool UObsidianInventoryWidgetController::CanPlaceDraggedItem(const FIntPoint& AtGridSlot) const
{
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is invalid in UObsidianInventoryWidgetController::CanPlaceDraggedItem."));
		return false;	
	}

	FIntPoint LocalItemGridSpan;
	if(!GetDraggedItemGridSpan(LocalItemGridSpan))
	{
		return false;
	}
	return InventoryComponent->CheckSpecifiedPosition(LocalItemGridSpan, AtGridSlot);
}

bool UObsidianInventoryWidgetController::CanPlaceDraggedItem(const FIntPoint& AtGridSlot, const FIntPoint& ItemGridSpan) const
{
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is invalid in UObsidianInventoryWidgetController::CanPlaceDraggedItem."));
		return false;	
	}
	
	return InventoryComponent->CheckSpecifiedPosition(ItemGridSpan, AtGridSlot);
}

bool UObsidianInventoryWidgetController::GetDraggedItemGridSpan(FIntPoint& OutItemGridSpan) const
{
	if(!IsDraggingAnItem())
	{
		return false;
	}
	
	const FDraggedItem DraggedItem = OwnerHeroComponent->GetDraggedItem();
	if(DraggedItem.IsEmpty())
	{
		return false;
	}
	
	if(const UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		OutItemGridSpan = Instance->GetItemGridSpan();
		return true;	
	}
	
	if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
		{
			if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
			{
				OutItemGridSpan = AppearanceFrag->GetItemGridSpanFromDesc();
				return true;	
			}
		}
	}
	return false;
}

UObsidianItem* UObsidianInventoryWidgetController::GetItemWidgetAtInventoryGridSlot(const FIntPoint& AtGridSlot) const
{
	if(AddedItemWidgetMap.Contains(AtGridSlot))
	{
		return AddedItemWidgetMap[AtGridSlot];
	}
	return nullptr;
}

void UObsidianInventoryWidgetController::RegisterInventoryItemWidget(const FIntPoint& GridSlot, UObsidianItem* ItemWidget)
{
	if(!AddedItemWidgetMap.Contains(GridSlot))
	{
		AddedItemWidgetMap.Add(GridSlot, ItemWidget);
	}
}

void UObsidianInventoryWidgetController::RemoveInventoryItemWidget(const FIntPoint& GridSlot)
{
	if(AddedItemWidgetMap.Contains(GridSlot))
	{
		if(UObsidianItem* Item = AddedItemWidgetMap[GridSlot])
		{
			if(ActiveItemDescription && ActiveItemDescription->IsInventoryItemDescription() && ActiveItemDescription->GetAssociatedInventoryLocation() == GridSlot)
			{
				RemoveItemDescription();
			}
			Item->RemoveFromParent();
		}
		AddedItemWidgetMap.Remove(GridSlot);
	}
}

UObsidianItem* UObsidianInventoryWidgetController::GetItemWidgetAtEquipmentSlot(const FGameplayTag& Slot) const
{
	if(EquippedItemWidgetMap.Contains(Slot))
	{
		return EquippedItemWidgetMap[Slot];
	}
	return nullptr;
}

void UObsidianInventoryWidgetController::RegisterEquipmentItemWidget(const FGameplayTag& Slot, UObsidianItem* ItemWidget, const bool bSwappedWithAnother)
{
	if(bSwappedWithAnother)
	{
		RemoveEquipmentItemWidget(Slot);
	}
	if(!EquippedItemWidgetMap.Contains(Slot))
	{
		EquippedItemWidgetMap.Add(Slot, ItemWidget);
	}
}

void UObsidianInventoryWidgetController::AddBlockedEquipmentItemWidget(const FGameplayTag& PrimarySlot, UObsidianSlotBlockadeItem* ItemWidget, const bool bSwappedWithAnother)
{
	if(bSwappedWithAnother)
	{
		RemoveBlockedSlotItemWidget(PrimarySlot);
	}
	if(!BlockedSlotsWidgetMap.Contains(PrimarySlot))
	{
		BlockedSlotsWidgetMap.Add(PrimarySlot, ItemWidget);
	}
}

void UObsidianInventoryWidgetController::RemoveEquipmentItemWidget(const FGameplayTag& Slot)
{
	if(EquippedItemWidgetMap.Contains(Slot))
	{
		if(UObsidianItem* Item = EquippedItemWidgetMap[Slot])
		{
			if(ActiveItemDescription && ActiveItemDescription->IsEquipmentDescription() && ActiveItemDescription->GetAssociatedSlotTag() == Slot)
			{
				RemoveItemDescription();
			}
			Item->RemoveFromParent();
		}
		EquippedItemWidgetMap.Remove(Slot);
	}
}

void UObsidianInventoryWidgetController::RemoveBlockedSlotItemWidget(const FGameplayTag& Slot)
{
	if(BlockedSlotsWidgetMap.Contains(Slot))
	{
		if(UObsidianSlotBlockadeItem* Item = BlockedSlotsWidgetMap[Slot])
		{
			if(UObsidianItemSlot_Equipment* BlockedSlot = Item->GetOwningSlot())
			{
				BlockedSlot->SetIsBlocked(false);
			}
			Item->RemoveFromParent();
		}
		BlockedSlotsWidgetMap.Remove(Slot);
	}
}

bool UObsidianInventoryWidgetController::CanEquipDraggedItem(const FGameplayTag& SlotTag) const
{
	if(OwnerHeroComponent == nullptr || EquipmentComponent == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("OwnerHeroComponent or EquipmentComponent is invalid in UObsidianInventoryWidgetController::CanEquipDraggedItem."))
		return false; 
	}

	const FDraggedItem DraggedItem = OwnerHeroComponent->GetDraggedItem();
	if(const UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance)
	{
		const EObsidianEquipResult EquipResult = EquipmentComponent->CanEquipInstance(DraggedInstance, SlotTag);
		return EquipResult == EObsidianEquipResult::CanEquip;
	}
	if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef)
	{
		const EObsidianEquipResult EquipResult = EquipmentComponent->CanEquipTemplate(DraggedItemDef, SlotTag);
		return EquipResult == EObsidianEquipResult::CanEquip;
	}
	return false;
}

FVector2D UObsidianInventoryWidgetController::CalculateUnstackSliderPosition(const UObsidianItem* ItemWidget) const
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Failed to calculate Unstack Slider Position"));
		return FVector2D::Zero();
	}

	if(ItemWidget == nullptr || ActiveUnstackSlider == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Failed to calculate Unstack Slider Position"));
		return FVector2D::Zero();
	}
	
	FVector2D SliderSize = ActiveUnstackSlider->GetSizeBoxSize();
		
	const FGeometry& CachedGeometry = ItemWidget->GetCachedGeometry();
	FVector2D ItemLocalSize = ItemWidget->GetItemWidgetSize();

	// Adjusting sizes based on viewport scale
	const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(World);
	SliderSize *= DPIScale; 
	ItemLocalSize *= DPIScale;

	FVector2D ItemPixelPosition = FVector2D::Zero();
	FVector2D ItemViewportPosition = FVector2D::Zero();
	USlateBlueprintLibrary::LocalToViewport(World, CachedGeometry, FVector2D(0.f,0.f), ItemPixelPosition, ItemViewportPosition);

	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(World);
	return GetItemUIElementPositionBoundByViewport(ViewportSize, ItemPixelPosition, ItemLocalSize, SliderSize);
}

FVector2D UObsidianInventoryWidgetController::CalculateDescriptionPosition(const UObsidianItem* ItemWidget) const
{
	FVector2D FinalPosition = FVector2D::Zero();
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Failed to calculate Description Position"));
		return FinalPosition;
	}

	if(ItemWidget == nullptr || ActiveItemDescription == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Failed to calculate Description Position"));
		return FinalPosition;
	}

	// @HACK this is quite ugly, but without prepass the desired size is [0, 0], if the performance is the problem,
	// I could delay the calculation for a frame and see how reliable it is to retrieve the sie information,
	// Other system with delegates could be implemented to get the size reliably, but it just needs testing cuz if it's not bad I don't really care for now.
	ActiveItemDescription->ForceLayoutPrepass();
	FVector2D DescriptionSize = ActiveItemDescription->GetDesiredSize();
		
	const FGeometry& CachedGeometry = ItemWidget->GetCachedGeometry();
	FVector2D ItemLocalSize = ItemWidget->GetItemWidgetSize();
		
	// Adjusting sizes based on viewport scale
	const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(World);
	ItemLocalSize *= DPIScale; 
	DescriptionSize *= DPIScale;
		
	FVector2D ItemPixelPosition = FVector2D::Zero();
	FVector2D ItemViewportPosition = FVector2D::Zero();
	USlateBlueprintLibrary::LocalToViewport(World, CachedGeometry, FVector2D(0.f,0.f), ItemPixelPosition, ItemViewportPosition);
	
	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(World);
	return GetItemUIElementPositionBoundByViewport(ViewportSize, ItemPixelPosition, ItemLocalSize, DescriptionSize);
}

FVector2D UObsidianInventoryWidgetController::GetItemUIElementPositionBoundByViewport(const FVector2D& ViewportSize, const FVector2D& ItemPosition, const FVector2D& ItemSize, const FVector2D& UIElementSize) const
{
	FVector2D BoundedPosition = FVector2D((ItemPosition.X - (UIElementSize.X / 2)) + (ItemSize.X / 2),
										  (ItemPosition.Y - UIElementSize.Y));
	
	const bool bFitsLeft = BoundedPosition.X > 0.0f;
	const bool bFitsRight = (BoundedPosition.X + UIElementSize.X) < ViewportSize.X;
	const bool bFitsTop = (BoundedPosition.Y - UIElementSize.Y) > 0.0f;
	
	if(bFitsLeft && bFitsRight && bFitsTop) // We fit in the default position [top-middle]
	{
		return BoundedPosition;
	}
	
	if(bFitsLeft && bFitsRight && bFitsTop == false)
	{
		BoundedPosition = FVector2D((ItemPosition.X - (UIElementSize.X / 2)) + (ItemSize.X / 2),(ItemPosition.Y + ItemSize.Y));
		if((BoundedPosition.Y + UIElementSize.Y) < ViewportSize.Y) // Desc fit below [bottom-middle]
		{
			return BoundedPosition;
		}
	}
	
	if(bFitsRight == false)
	{
		BoundedPosition = FVector2D((ItemPosition.X - UIElementSize.X),(ItemPosition.Y - (UIElementSize.Y / 2)) + (ItemSize.Y / 2));
		if(BoundedPosition.X > 0.0f) // Desc fit left [left-middle]
		{
			return BoundedPosition;
		}
	}
	
	if(bFitsLeft == false)
	{
		BoundedPosition = FVector2D((ItemPosition.X + ItemSize.X),(ItemPosition.Y - (UIElementSize.Y / 2)) + (ItemSize.Y / 2));
		if((BoundedPosition.X + UIElementSize.X) < ViewportSize.X) // Desc Fit right [right-middle]
		{
			return BoundedPosition;
		}
	}
	
	// Falling back to the default not so happy position which is most likely to fit if all above cases fail,
	// could improve it later to fit the screen in every case but is not necessary now [middle-middle].
	BoundedPosition = FVector2D((ItemPosition.X - (UIElementSize.X / 2)) + (ItemSize.X / 2),(ItemPosition.Y - (UIElementSize.Y / 2)) + (ItemSize.Y / 2));
	
	return BoundedPosition;
}


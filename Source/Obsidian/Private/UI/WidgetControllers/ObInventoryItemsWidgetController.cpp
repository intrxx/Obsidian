// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"

#include <GameFramework/GameplayMessageSubsystem.h>
#include <Blueprint/SlateBlueprintLibrary.h>
#include <Blueprint/WidgetLayoutLibrary.h>

#include "UI/Inventory/Items/ObsidianItemDescriptionBase.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "CharacterComponents/ObsidianPlayerInputManager.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/ObsidianHUD.h"
#include "InventoryItems/Equipment/ObsidianEquipmentList.h"
#include "InventoryItems/PlayerStash/ObsidianStashTab.h"
#include "UI/Inventory/Items/ObsidianDraggedItem.h"
#include "UI/Inventory/Items/ObsidianItem.h"
#include "Obsidian/Public/UI/Inventory/Items/ObsidianUnstackSlider.h"
#include "UI/MainOverlay/ObsidianMainOverlay.h"

DEFINE_LOG_CATEGORY(LogWidgetController_Items);

// ~ Start of FObsidianItemWidgetData

bool FObsidianItemWidgetData::IsItemForSwapSlot() const 
{
	const FGameplayTag DesiredSlot = ItemPosition.GetItemSlotTag();
	if(DesiredSlot.IsValid() && DesiredSlot.MatchesTag(FGameplayTag::RequestGameplayTag("Item.SwapSlot.Equipment",
		true)))
	{
		return true;
	}
	return false;
}

// ~ End of FObsidianItemWidgetData

void UObInventoryItemsWidgetController::OnWidgetControllerSetupCompleted()
{
	check(InventoryComponent);
	check(EquipmentComponent);
	check(ObsidianPlayerController);
	check(PlayerStashComponent);
	
	const AActor* OwningActor = Cast<AActor>(ObsidianPlayerController->GetPawn());
	check(OwningActor);
	
	OwnerPlayerInputManager = UObsidianPlayerInputManager::FindPlayerInputManager(OwningActor);
	check(OwnerPlayerInputManager);
	OwnerPlayerInputManager->OnStopUsingItemDelegate.AddUObject(this, &ThisClass::ClearUsableUIContext);
	OwnerPlayerInputManager->OnStartDraggingItemDelegate.AddUObject(this, &ThisClass::OnStartDraggingItem);
	OwnerPlayerInputManager->OnStopDraggingItemDelegate.AddUObject(this, &ThisClass::OnStopDraggingItem);

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwningActor->GetWorld());
	MessageSubsystem.RegisterListener(ObsidianGameplayTags::Message_Inventory_Changed, this, &ThisClass::OnInventoryStateChanged);
	MessageSubsystem.RegisterListener(ObsidianGameplayTags::Message_Equipment_Changed, this, &ThisClass::OnEquipmentStateChanged);
	MessageSubsystem.RegisterListener(ObsidianGameplayTags::Message_PlayerStash_Changed, this, &ThisClass::OnPlayerStashChanged);

	//RegisterInitialStashTabs(); //TODO(intrxx) Stash refactor, to check
}

void UObInventoryItemsWidgetController::OnInventoryStateChanged(FGameplayTag Channel, const FObsidianInventoryChangeMessage& InventoryChangeMessage)
{
	if(InventoryComponent != InventoryChangeMessage.InventoryOwner) // Fixes a bug when Items appear in Server's Inventory (Listen Server Character) after picked up by client.
	{
		return;
	}
	
	const UObsidianInventoryItemInstance* Instance = InventoryChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Item Instance is invalid in [%hs]"), __FUNCTION__);
		return;
	}

	if(InventoryChangeMessage.ChangeType == EObsidianInventoryChangeType::ICT_GeneralItemChanged)
	{
		//TODO(intrxx) Fix highlight
		// HandleHoveringOverItem(InventoryChangeMessage.GridItemPosition, nullptr);
		// if(UObsidianItem* CorrespondingItemWidget = GetItemWidgetFromInventoryAtGridPosition(InventoryChangeMessage.GridItemPosition))
		// {
		// 	CorrespondingItemWidget->ResetHighlight();
		// 	CachedItemsMatchingUsableContext.Remove(CorrespondingItemWidget);
		// }
	}
	else if(InventoryChangeMessage.ChangeType == EObsidianInventoryChangeType::ICT_ItemAdded)
	{
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Adding item: [%s] to Inventory"), *Instance->GetItemDisplayName().ToString());
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemImage = Instance->GetItemImage();
		ItemWidgetData.ItemPosition = InventoryChangeMessage.GridItemPosition;
		ItemWidgetData.GridSpan = Instance->GetItemGridSpan();
		ItemWidgetData.StackCount = Instance->IsStackable() ? InventoryChangeMessage.NewCount : 0;
		ItemWidgetData.bUsable = Instance->IsItemUsable();
		ItemWidgetData.ItemSlotPadding = Instance->GetItemSlotPadding();
		
		OnItemInventorizedDelegate.Broadcast(ItemWidgetData);
	}
	else if(InventoryChangeMessage.ChangeType == EObsidianInventoryChangeType::ICT_ItemRemoved)
	{
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Removing item: [%s] from Inventory"), *Instance->GetItemDisplayName().ToString());
		
		if(OwnerPlayerInputManager->IsUsingItem())
		{
			if(InventoryChangeMessage.ItemInstance == OwnerPlayerInputManager->GetUsingItem())
			{
				OwnerPlayerInputManager->SetUsingItem(false);  //TODO(intrxx) This probably shouldn't be here, its widget controller
			}
		}
		
		ClearItemDescriptionForPosition(InventoryChangeMessage.GridItemPosition);

		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemPosition = InventoryChangeMessage.GridItemPosition;
		OnInventorizedItemRemovedDelegate.Broadcast(ItemWidgetData);
	}
	else if (InventoryChangeMessage.ChangeType == EObsidianInventoryChangeType::ICT_ItemStacksChanged)
	{
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Changing item: [%s] in Inventory"), *Instance->GetItemDisplayName().ToString());
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemPosition = InventoryChangeMessage.GridItemPosition;
		ItemWidgetData.StackCount = Instance->IsStackable() ? InventoryChangeMessage.NewCount : 0;
		
		OnInventoryItemChangedDelegate.Broadcast(ItemWidgetData);
	}
}

void UObInventoryItemsWidgetController::OnEquipmentStateChanged(FGameplayTag Channel, const FObsidianEquipmentChangeMessage& EquipmentChangeMessage)
{
	if(EquipmentComponent != EquipmentChangeMessage.EquipmentOwner) // Fixes a bug when Items appear in Server's Inventory (Listen Server Character) after picked up by client.
	{
		return;
	}
	
	const UObsidianInventoryItemInstance* Instance = EquipmentChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Inventory Item Instance is invalid in [%hs]"), __FUNCTION__);
		return;
	}

	if(EquipmentChangeMessage.ChangeType == EObsidianEquipmentChangeType::ECT_ItemEquipped)
	{
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Equipping item: [%s]"), *Instance->GetItemDisplayName().ToString());

		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemImage = Instance->GetItemImage();
		ItemWidgetData.ItemPosition = EquipmentChangeMessage.SlotTag;
		ItemWidgetData.GridSpan = Instance->GetItemGridSpan();
		ItemWidgetData.bDoesBlockSisterSlot = Instance->DoesItemNeedTwoSlots();
		ItemWidgetData.ItemCategory = Instance->GetItemCategoryTag();
		ItemWidgetData.ItemSlotPadding = Instance->GetItemSlotPadding();
		
		OnItemEquippedDelegate.Broadcast(ItemWidgetData);
	}
	else if(EquipmentChangeMessage.ChangeType == EObsidianEquipmentChangeType::ECT_ItemUnequipped)
	{
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Unequipping item: [%s]"), *Instance->GetItemDisplayName().ToString());
		
		const FGameplayTag SlotTagToClear = EquipmentChangeMessage.SlotTagToClear;
		if(SlotTagToClear.IsValid())
		{
			ClearItemDescriptionForPosition(SlotTagToClear);

			FObsidianItemWidgetData ItemWidgetData;
			ItemWidgetData.ItemPosition = SlotTagToClear;
			ItemWidgetData.bDoesBlockSisterSlot = Instance->DoesItemNeedTwoSlots();
			OnEquippedItemRemovedDelegate.Broadcast(ItemWidgetData);
		}
	}
	else if(EquipmentChangeMessage.ChangeType == EObsidianEquipmentChangeType::ECT_ItemSwapped)
	{
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Equipment Swapping item: [%s]"), *Instance->GetItemDisplayName().ToString());

		const FGameplayTag SlotTagToClear = EquipmentChangeMessage.SlotTagToClear;
		if(SlotTagToClear.IsValid())
		{
			FObsidianItemWidgetData ItemWidgetData;
			ItemWidgetData.ItemPosition = SlotTagToClear;
			ItemWidgetData.bDoesBlockSisterSlot = Instance->DoesItemNeedTwoSlots();
			OnEquippedItemRemovedDelegate.Broadcast(ItemWidgetData);
		}
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemImage = Instance->GetItemImage();
		ItemWidgetData.ItemPosition = EquipmentChangeMessage.SlotTag;
		ItemWidgetData.GridSpan = Instance->GetItemGridSpan();
		ItemWidgetData.bSwappedWithAnotherItem = EquipmentChangeMessage.SlotTagToClear == FGameplayTag::EmptyTag;
		ItemWidgetData.bDoesBlockSisterSlot = Instance->DoesItemNeedTwoSlots();
		ItemWidgetData.ItemCategory = Instance->GetItemCategoryTag();
		ItemWidgetData.ItemSlotPadding = Instance->GetItemSlotPadding();
		
		OnItemEquippedDelegate.Broadcast(ItemWidgetData);
	}
}

void UObInventoryItemsWidgetController::OnPlayerStashChanged(FGameplayTag Channel, const FObsidianStashChangeMessage& StashChangeMessage)
{
	if(PlayerStashComponent != StashChangeMessage.PlayerStashOwner) // Fixes a bug when Items appear in Server's Inventory (Listen Server Character) after picked up by client.
	{
		return;
	}
	
	const UObsidianInventoryItemInstance* Instance = StashChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Item Instance is invalid in [%hs]"), __FUNCTION__);
		return;
	}

	if(StashChangeMessage.ChangeType == EObsidianStashChangeType::ICT_GeneralItemChanged)
	{
		// HandleHoveringOverItem(StashChangeMessage.GridItemPosition);
		// if(UObsidianItem* CorrespondingItemWidget = GetItemWidgetFromInventoryAtGridPosition(InventoryChangeMessage.GridItemPosition))
		// {
		// 	CorrespondingItemWidget->ResetHighlight();
		// 	CachedItemsMatchingUsableContext.Remove(CorrespondingItemWidget);
		// }
	}
	else if(StashChangeMessage.ChangeType == EObsidianStashChangeType::ICT_ItemAdded)
	{
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Adding item: [%s] to Player Stash"), *Instance->GetItemDisplayName().ToString());
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemImage = Instance->GetItemImage();
		ItemWidgetData.ItemPosition = StashChangeMessage.ItemPosition;
		ItemWidgetData.GridSpan = Instance->GetItemGridSpan();
		ItemWidgetData.StackCount = Instance->IsStackable() ? StashChangeMessage.NewCount : 0;
		ItemWidgetData.bUsable = Instance->IsItemUsable();
		ItemWidgetData.ItemSlotPadding = Instance->GetItemSlotPadding();
		
		OnItemStashedDelegate.Broadcast(ItemWidgetData);
	}
	else if(StashChangeMessage.ChangeType == EObsidianStashChangeType::ICT_ItemRemoved)
	{
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Removing item: [%s] from Player Stash"), *Instance->GetItemDisplayName().ToString());
		//RemoveStashItemWidget(StashChangeMessage.ItemPosition);
		
		if(OwnerPlayerInputManager->IsUsingItem())
		{
			if(StashChangeMessage.ItemInstance == OwnerPlayerInputManager->GetUsingItem())
			{
				OwnerPlayerInputManager->SetUsingItem(false);  //TODO(intrxx) This probably shouldn't be here, its widget controller
			}
		}

		ClearItemDescriptionForPosition(StashChangeMessage.ItemPosition);
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemPosition = StashChangeMessage.ItemPosition;
		OnStashedItemRemovedDelegate.Broadcast(ItemWidgetData);
	}
	else if (StashChangeMessage.ChangeType == EObsidianStashChangeType::ICT_ItemStacksChanged)
	{
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Changing item: [%s] in Player Stash"), *Instance->GetItemDisplayName().ToString());
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemPosition = StashChangeMessage.ItemPosition;
		ItemWidgetData.StackCount = Instance->IsStackable() ? StashChangeMessage.NewCount : 0;
		
		OnStashedItemChangedDelegate.Broadcast(ItemWidgetData);
	}
}

void UObInventoryItemsWidgetController::OnStartDraggingItem(const FDraggedItem& DraggedItem)
{
	if (ObsidianPlayerController == nullptr || DraggedItem.IsEmpty())
	{
		return;
	}
	
	const AObsidianHUD* ObsidianHUD = ObsidianPlayerController->GetObsidianHUD();
	if (ObsidianHUD == nullptr)
	{
		return;
	}
	
	FGameplayTagContainer JoinedSlotTags;
	
	if (EquipmentComponent && ObsidianHUD->IsInventoryOpened()) // Gather possible equipment slots
	{
		if (const UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance)
		{
			for (const FObsidianEquipmentSlotDefinition& EquipmentSlot : EquipmentComponent->FindPossibleSlotsForEquipping_WithInstance(DraggedInstance))
			{
				JoinedSlotTags.AddTag(EquipmentSlot.GetEquipmentSlotTag());
			}
		}
		else if (const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef = DraggedItem.ItemDef)
		{
			for (const FObsidianEquipmentSlotDefinition& EquipmentSlot : EquipmentComponent->FindPossibleSlotsForEquipping_WithItemDef(ItemDef, DraggedItem.GeneratedData))
			{
				JoinedSlotTags.AddTag(EquipmentSlot.GetEquipmentSlotTag());
			}
		}
	}

	FGameplayTagContainer StashMatchingSlotTags;
	if (PlayerStashComponent && ObsidianHUD->IsPlayerStashOpened()) // Gather possible functional slots
	{
		if (const UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance)
		{
			for (const FObsidianStashSlotDefinition& StashSlot : PlayerStashComponent->FindPossibleSlotsForPlacingItem_WithInstance(DraggedInstance))
			{
				JoinedSlotTags.AddTag(StashSlot.GetStashSlotTag());
			}
		}
		else if (const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef = DraggedItem.ItemDef)
		{
			for (const FObsidianStashSlotDefinition& StashSlot : PlayerStashComponent->FindPossibleSlotsForPlacingItem_WithItemDef(ItemDef))
			{
				JoinedSlotTags.AddTag(StashSlot.GetStashSlotTag());
			}
		}
	}
	
	OnStartPlacementHighlightDelegate.Broadcast(JoinedSlotTags);
}

void UObInventoryItemsWidgetController::OnStopDraggingItem()
{
	OnStopPlacementHighlightDelegate.Broadcast();
}

UObsidianItemDescriptionBase* UObInventoryItemsWidgetController::GetActiveDroppedItemDescription()
{
	return ActiveDroppedItemDescription;
}

TConstArrayView<TObjectPtr<UObsidianStashTab>> UObInventoryItemsWidgetController::GetAllStashTabs() const
{
	check(PlayerStashComponent);
	return PlayerStashComponent->GetAllStashTabs();
}

int32 UObInventoryItemsWidgetController::GetInventoryGridWidth() const
{
	if(InventoryComponent)
	{
		return InventoryComponent->GetInventoryGridWidth();
	}
	
	UE_LOG(LogWidgetController_Items, Error, TEXT("Trying to return Grid Width but Inventory Component is invalid in [%hs]"), __FUNCTION__);
	return 0;
}

int32 UObInventoryItemsWidgetController::GetInventoryGridHeight() const
{
	if(InventoryComponent)
	{
		return InventoryComponent->GetInventoryGridHeight();
	}
	
	UE_LOG(LogWidgetController_Items, Error, TEXT("Trying to return Grid Height but Inventory Component is invalid in [%hs]"), __FUNCTION__);
	return 0;
}

void UObInventoryItemsWidgetController::OnInventoryOpen()
{
	TArray<UObsidianInventoryItemInstance*> InventoryItems = InventoryComponent->GetAllItems();
	
	for(const UObsidianInventoryItemInstance* Item : InventoryItems)
	{
		if(ensure(Item))
		{
			FObsidianItemWidgetData ItemWidgetData;
			ItemWidgetData.ItemImage = Item->GetItemImage();
			ItemWidgetData.GridSpan = Item->GetItemGridSpan();
			ItemWidgetData.ItemPosition = Item->GetItemCurrentPosition();
			ItemWidgetData.StackCount = Item->IsStackable() ? Item->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current) : 0;
			ItemWidgetData.bUsable = Item->IsItemUsable();
			ItemWidgetData.ItemSlotPadding = Item->GetItemSlotPadding();

			OnItemInventorizedDelegate.Broadcast(ItemWidgetData);
		}
	}

	TArray<UObsidianInventoryItemInstance*> EquippedItems = EquipmentComponent->GetAllEquippedItems();
	
	for(const UObsidianInventoryItemInstance* Item : EquippedItems)
	{
		if(ensure(Item))
		{
			FObsidianItemWidgetData ItemWidgetData;
			ItemWidgetData.ItemImage = Item->GetItemImage();
			ItemWidgetData.GridSpan = Item->GetItemGridSpan();
			ItemWidgetData.ItemPosition = Item->GetItemCurrentPosition();
			ItemWidgetData.bDoesBlockSisterSlot = Item->DoesItemNeedTwoSlots();
			ItemWidgetData.ItemSlotPadding = Item->GetItemSlotPadding();
		
			OnItemEquippedDelegate.Broadcast(ItemWidgetData);
		}
	}
}

void UObInventoryItemsWidgetController::OnPlayerStashOpen()
{
	UE_LOG(LogTemp, Display, TEXT("Stash Opened."));

	//TODO(intrxx) This for sure will need to be changed, it will be to heavy on performance.
	TArray<UObsidianInventoryItemInstance*> StashedItems = PlayerStashComponent->GetAllItems();
	
	/*
	 *TODO I need to rethink that, I might not need to reset these items since the Player Stash state
	 *	might not change without Player seeing it (but what about Stash Tab Affinities?).
	 **/
	//EmptyRegisteredItems();
	
	for(const UObsidianInventoryItemInstance* Instance : StashedItems)
	{
		if(ensure(Instance))
		{
			FObsidianItemWidgetData ItemWidgetData;
			ItemWidgetData.ItemImage = Instance->GetItemImage();
			ItemWidgetData.ItemPosition = Instance->GetItemCurrentPosition();
			ItemWidgetData.GridSpan = Instance->GetItemGridSpan();
			ItemWidgetData.StackCount = Instance->IsStackable() ? Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current) : 0;
			ItemWidgetData.bUsable = Instance->IsItemUsable();
			ItemWidgetData.ItemSlotPadding = Instance->GetItemSlotPadding();
		
			OnItemStashedDelegate.Broadcast(ItemWidgetData);
		}
	}
}

void UObInventoryItemsWidgetController::RequestAddingItemToInventory(const FIntPoint& ToGridSlot, const bool bShiftDown)
{
	check(OwnerPlayerInputManager);

	if(OwnerPlayerInputManager->IsUsingItem())
	{
		StopUsingItem();
		return;
	}
	
	if(OwnerPlayerInputManager->IsDraggingAnItem() == false)
	{
		return;
	}
	OwnerPlayerInputManager->ServerAddItemToInventoryAtSlot(ToGridSlot, bShiftDown);
}

void UObInventoryItemsWidgetController::RequestAddingItemToEquipment(const FGameplayTag& SlotTag)
{
	check(OwnerPlayerInputManager);

	if(OwnerPlayerInputManager->IsUsingItem())
	{
		StopUsingItem();
		return;
	}
	
	if(OwnerPlayerInputManager->IsDraggingAnItem() == false)
	{
		return;
	}
	OwnerPlayerInputManager->ServerEquipItemAtSlot(SlotTag);
}

void UObInventoryItemsWidgetController::RequestAddingItemToStashTab(const FObsidianItemPosition& ToPosition, const bool bShiftDown)
{
	check(OwnerPlayerInputManager);

	if(OwnerPlayerInputManager->IsUsingItem())
	{
		StopUsingItem();
		return;
	}
	
	if(OwnerPlayerInputManager->IsDraggingAnItem() == false)
	{
		return;
	}
	OwnerPlayerInputManager->ServerAddItemToStashTabAtSlot(ToPosition, bShiftDown);
}

void UObInventoryItemsWidgetController::RequestAddingItem(const FObsidianItemPosition& AtItemPosition,
	const FObsidianItemInteractionData& InteractionData, const EObsidianPanelOwner PanelOwner)
{
	if (IsDraggingAnItem() == false)
	{
		return;
	}
	
	check((uint8)PanelOwner > 0);
	switch (PanelOwner)
	{
		case EObsidianPanelOwner::Inventory:
			{
				ensureMsgf(AtItemPosition.IsOnInventoryGrid(), TEXT("Trying to add item to Inventory with"
														" invalid Inventory position."));

				const bool bShiftDown = InteractionData.InteractionFlags.bItemStacksInteraction;
				RequestAddingItemToInventory(AtItemPosition.GetItemGridPosition(), bShiftDown);
				break;
			}
		case EObsidianPanelOwner::Equipment:
			{
				ensureMsgf(AtItemPosition.IsOnEquipmentSlot(), TEXT("Trying to add item to Equipment with"
														" invalid Equipment position."));
				
				RequestAddingItemToEquipment(AtItemPosition.GetItemSlotTag());
				break;
			}
		case EObsidianPanelOwner::PlayerStash:
			{
				ensureMsgf(AtItemPosition.IsOnStash(), TEXT("Trying to add item to Stash with"
														" invalid Stash position."));

				const bool bShiftDown = InteractionData.InteractionFlags.bItemStacksInteraction;
				RequestAddingItemToStashTab(AtItemPosition, bShiftDown);
				break;
			}
			default:
			{
				UE_LOG(LogWidgetController_Items, Error, TEXT("[%d] PanelOwner is invalid in [%hs]."),
					PanelOwner, __FUNCTION__);
				break;
			}
	}
}

void UObInventoryItemsWidgetController::HandleRightClickingOnItem(const FObsidianItemPosition& AtItemPosition,
	const FObsidianItemInteractionData& InteractionData, const EObsidianPanelOwner PanelOwner)
{
	check((uint8)PanelOwner > 0);
	switch (PanelOwner)
	{
		case EObsidianPanelOwner::Inventory:
			{
				ensureMsgf(AtItemPosition.IsOnInventoryGrid(), TEXT("Trying to handle right click on Inventory"
														" item with invalid Inventory position."));
				
				HandleRightClickingOnInventoryItem(AtItemPosition.GetItemGridPosition(), InteractionData.ItemWidget);
				break;
			}
		case EObsidianPanelOwner::PlayerStash:
			{
				ensureMsgf(AtItemPosition.IsOnStash(), TEXT("Trying to handle right click on Stash item with"
														" invalid Stash position."));
				HandleRightClickingOnStashedItem(AtItemPosition, InteractionData.ItemWidget);
				break;
			}
			default:
				{
					UE_LOG(LogWidgetController_Items, Error, TEXT("[%d] PanelOwner is invalid in [%hs]."),
						PanelOwner, __FUNCTION__);
					break;
				}
	}
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnItem(const FObsidianItemPosition& AtItemPosition,
	const FObsidianItemInteractionData& InteractionData, const EObsidianPanelOwner PanelOwner)
{
	check((uint8)PanelOwner > 0);
	switch (PanelOwner)
	{
		case EObsidianPanelOwner::Inventory:
			{
				ensureMsgf(AtItemPosition.IsOnInventoryGrid(), TEXT("Trying to handle left click on Inventorized"
														" item with invalid Inventory position."));
				
				if(InteractionData.InteractionFlags.bItemStacksInteraction)
				{
					HandleLeftClickingOnInventoryItemWithShiftDown(AtItemPosition.GetItemGridPosition(),
						InteractionData.ItemWidget);
				}
				else
				{
					HandleLeftClickingOnInventoryItem(AtItemPosition.GetItemGridPosition(),
						InteractionData.InteractionFlags.bMoveBetweenNextOpenedWindow);	
				}
				
				break;
			}
		case EObsidianPanelOwner::Equipment:
			{
				ensureMsgf(AtItemPosition.IsOnEquipmentSlot(), TEXT("Trying to handle left click on Equipped"
														" item with invalid Equipment position."));

				if (InteractionData.InteractionFlags.bInteractWithSisterSlottedItem)
				{
					HandleLeftClickingOnEquipmentItem(AtItemPosition.GetItemSlotTag(),
						InteractionData.InteractionTargetPositionOverride.GetItemSlotTag());
				}
				else
				{
					HandleLeftClickingOnEquipmentItem(AtItemPosition.GetItemSlotTag());
				}
				
				break;
			}
		case EObsidianPanelOwner::PlayerStash:
			{
				ensureMsgf(AtItemPosition.IsOnStash(), TEXT("Trying to handle left click on Stashed"
												" item with invalid Stash position."));

				if (InteractionData.InteractionFlags.bItemStacksInteraction)
				{
					HandleLeftClickingOnStashedItemWithShiftDown(AtItemPosition, InteractionData.ItemWidget);
				}
				else
				{
					HandleLeftClickingOnStashedItem(AtItemPosition,
						InteractionData.InteractionFlags.bMoveBetweenNextOpenedWindow);
				}
				
				break;
			}
			default:
				{
					UE_LOG(LogWidgetController_Items, Error, TEXT("[%d] PanelOwner is invalid in [%hs]."),
						PanelOwner, __FUNCTION__);
					break;
				}
	}
}

void UObInventoryItemsWidgetController::HandleRightClickingOnInventoryItem(const FIntPoint& AtGridSlot,
	UObsidianItem* ItemWidget)
{
	check(OwnerPlayerInputManager);
	if(OwnerPlayerInputManager && OwnerPlayerInputManager->IsDraggingAnItem())
	{
		return;
	}
	
	check(InventoryComponent);
	if(InventoryComponent && InventoryComponent->CanOwnerModifyInventoryState() == false)
	{
		return;
	}
	
	UObsidianInventoryItemInstance* UsingInstance = InventoryComponent->GetItemInstanceAtLocation(AtGridSlot);
	if(UsingInstance && UsingInstance->IsItemUsable() == false)
	{
		return;
	}

	if(UsingInstance->GetUsableItemType() == EObsidianUsableItemType::UIT_Crafting)
	{
		ItemWidget = ItemWidget == nullptr ? GetItemWidgetFromInventoryAtGridPosition(AtGridSlot) : ItemWidget;
		if (ItemWidget == nullptr)
		{
			UE_LOG(LogWidgetController_Items, Error, TEXT("ItemWidget is invalid in [%hs]."), __FUNCTION__);
			return;
		}
		
		OwnerPlayerInputManager->SetUsingItem(true, ItemWidget, UsingInstance);

		TArray<UObsidianInventoryItemInstance*> AllItems;
		AllItems.Append(InventoryComponent->GetAllItems());
		AllItems.Append(EquipmentComponent->GetAllEquippedItems());
		AllItems.Append(PlayerStashComponent->GetAllItems()); //TODO(intrxx) This will be hella slow, change later 
	
		const FObsidianItemsMatchingUsableContext MatchingUsableContext = UsingInstance->FireItemUseUIContext(AllItems);
		for(const FIntPoint& GridLocation : MatchingUsableContext.InventoryItemsMatchingContext)
		{
			//TODO(intrxx) implement hightlight
			// if(UObsidianItem* Item = GetItemWidgetFromInventoryAtGridPosition(GridLocation))
			// {
			// 	Item->HighlightItem();
			// 	CachedItemsMatchingUsableContext.Add(Item);
			// }
		}
		for(const FGameplayTag& SlotTag : MatchingUsableContext.EquipmentItemsMatchingContext)
		{
			//TODO(intrxx) implement hightlight
			// if(UObsidianItem* Item = GetItemWidgetAtEquipmentSlot(SlotTag))
			// {
			// 	Item->HighlightItem();
			// 	CachedItemsMatchingUsableContext.Add(Item);
			// }
		}
		for(const FObsidianItemPosition& StashPosition : MatchingUsableContext.StashItemsMatchingContext)
		{
			// if (UObsidianItem* Item = GetItemWidgetAtStashPosition(StashPosition))
			// {
			// 	Item->HighlightItem();
			// 	CachedItemsMatchingUsableContext.Add(Item);
			// }
		}
	}
	else if(UsingInstance->GetUsableItemType() == EObsidianUsableItemType::UIT_Activation)
	{
		OwnerPlayerInputManager->ServerActivateUsableItemFromInventory(UsingInstance);
	}
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnInventoryItem(const FIntPoint& AtGridSlot, const bool bAddToOtherWindow)
{
	check(InventoryComponent);
	check(OwnerPlayerInputManager);
	check(DraggedItemWidgetClass);

	if(InventoryComponent->CanOwnerModifyInventoryState() == false)
	{
		return;
	}
	
	RemoveItemUIElements();

	if(OwnerPlayerInputManager->IsUsingItem())
	{
		OwnerPlayerInputManager->UseItem(AtGridSlot, false);
		return;
	}

	if (OwnerPlayerInputManager->IsDraggingAnItem() == false)
	{
		if (bAddToOtherWindow == false)
		{
			OwnerPlayerInputManager->ServerGrabInventoryItemToCursor(AtGridSlot);
			return;
		}

		AObsidianHUD* ObsidianHUD = ObsidianPlayerController->GetObsidianHUD();
		if (ObsidianHUD && ObsidianHUD->IsPlayerStashOpened()) //TODO(intrxx) For now I support only Inventory <-> Stash
		{
			const FGameplayTag ToStashTab = ObsidianHUD->GetActiveStashTabTag(); //TODO(intrxx) This will need updating when I will support Stash Tab Affinities
			OwnerPlayerInputManager->ServerTransferItemToPlayerStash(AtGridSlot, ToStashTab);
		}
		return;
	}
	
	const UObsidianInventoryItemInstance* InstanceToAddTo = InventoryComponent->GetItemInstanceAtLocation(AtGridSlot);
	if (InstanceToAddTo == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Item Instance at pressed Location is invalid in [%hs]"), __FUNCTION__);
		return;
	}
		
	const FDraggedItem DraggedItem = OwnerPlayerInputManager->GetDraggedItem();
	if (UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance) // We carry item instance.
	{
		if (DraggedInstance->IsStackable() && UObsidianItemsFunctionLibrary::IsTheSameItem(DraggedInstance, InstanceToAddTo))
		{
			OwnerPlayerInputManager->ServerAddStacksFromDraggedItemToInventoryItemAtSlot(AtGridSlot);
		}
		else if (InventoryComponent->CanReplaceItemAtSpecificSlotWithInstance(AtGridSlot, DraggedInstance))
		{
			OwnerPlayerInputManager->ServerReplaceItemAtInventorySlot(AtGridSlot);
		}
	}
	else if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef) // We carry item def
	{
		const UObsidianInventoryItemDefinition* DefaultObject = DraggedItemDef.GetDefaultObject();
		if (DefaultObject && DefaultObject->IsStackable() && UObsidianItemsFunctionLibrary::IsTheSameItem_WithDef(InstanceToAddTo, DraggedItemDef))
		{
			OwnerPlayerInputManager->ServerAddStacksFromDraggedItemToInventoryItemAtSlot(AtGridSlot);
		}
		else if (InventoryComponent->CanReplaceItemAtSpecificSlotWithDef(AtGridSlot, DraggedItemDef, DraggedItem.GeneratedData.AvailableStackCount))
		{
			OwnerPlayerInputManager->ServerReplaceItemAtInventorySlot(AtGridSlot);
		}
	}
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnInventoryItemWithShiftDown(const FIntPoint& AtGridSlot,
	const UObsidianItem* ItemWidget)
{
	check(OwnerPlayerInputManager);

	if(InventoryComponent->CanOwnerModifyInventoryState() == false)
	{
		return;
	}
	
	if(OwnerPlayerInputManager->IsUsingItem())
	{
		OwnerPlayerInputManager->UseItem(AtGridSlot, true);
		return;
	}
	
	if(OwnerPlayerInputManager->IsDraggingAnItem())
	{
		OwnerPlayerInputManager->ServerAddStacksFromDraggedItemToInventoryItemAtSlot(AtGridSlot, 1);
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

	checkf(UnstackSliderClass, TEXT("Tried to create widget without valid widget class in fill it in UObInventoryItemsWidgetController instance."));
	ActiveUnstackSlider = CreateWidget<UObsidianUnstackSlider>(ObsidianPlayerController, UnstackSliderClass);
	ActiveUnstackSlider->InitializeUnstackSlider(CurrentItemStacks, AtGridSlot);

	const FVector2D UnstackSliderViewportPosition = CalculateUnstackSliderPosition(ItemWidget);
	ActiveUnstackSlider->SetPositionInViewport(UnstackSliderViewportPosition);
	ActiveUnstackSlider->AddToViewport();
	bUnstackSliderActive = true;
	
	ActiveUnstackSlider->OnAcceptButtonPressedDelegate.AddUObject(this, &ThisClass::HandleTakingOutStacksFromInventory);
	ActiveUnstackSlider->OnCloseButtonPressedDelegate.AddUObject(this, &ThisClass::RemoveUnstackSlider);
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnEquipmentItem(const FGameplayTag& SlotTag, const FGameplayTag& EquipSlotTagOverride)
{
	const FGameplayTag SwapSlotTag = FGameplayTag::RequestGameplayTag(TEXT("Item.SwapSlot.Equipment"));
	if(SlotTag.MatchesTag(SwapSlotTag))
	{
		//TODO(intrxx) Cannot left-click on swapped item, add VO?
		return;
	}
	
	check(EquipmentComponent);
	check(OwnerPlayerInputManager);
	check(DraggedItemWidgetClass);
	
	if(EquipmentComponent->CanOwnerModifyEquipmentState() == false)
	{
		return;
	}
	
	RemoveItemUIElements();

	if(OwnerPlayerInputManager->IsUsingItem())
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("As of now it is imposible to use items onto equipped items, maybe will support it in the future."));
		StopUsingItem();
		return;
	}
	
	if(OwnerPlayerInputManager->IsDraggingAnItem()) // If we carry an item, try to replace it with it.
	{
		
		 const FDraggedItem DraggedItem = OwnerPlayerInputManager->GetDraggedItem();
		 if(UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance) // We carry item instance.
		 {
		 	const EObsidianEquipCheckResult EquipmentResult = EquipmentComponent->CanReplaceInstance(DraggedInstance, SlotTag);
		 	if(EquipmentResult == EObsidianEquipCheckResult::CanEquip)
		 	{
		 		OwnerPlayerInputManager->ServerReplaceItemAtEquipmentSlot(SlotTag, EquipSlotTagOverride);
		 	}
		 	else
		 	{
		 		//TODO(intrxx) Send Client RPC with some voice over passing EquipResult?
#if !UE_BUILD_SHIPPING
		 		UE_LOG(LogEquipment, Warning, TEXT("Item cannot be equipped, reason: [%s]"), *ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipmentResult));
#endif
		 	}
		 	return;
		 }
		
		 if(const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef) // We carry item def
		 {
		 	const EObsidianEquipCheckResult EquipmentResult = EquipmentComponent->CanReplaceTemplate(DraggedItemDef, SlotTag, DraggedItem.GeneratedData);
		 	if(EquipmentResult == EObsidianEquipCheckResult::CanEquip)
		 	{
		 		OwnerPlayerInputManager->ServerReplaceItemAtEquipmentSlot(SlotTag, EquipSlotTagOverride);
		 	}
		 	else
		 	{
		 		//TODO(intrxx) Send Client RPC with some voice over passing EquipResult?
#if !UE_BUILD_SHIPPING
			 	UE_LOG(LogEquipment, Warning, TEXT("Item cannot be equipped, reason: [%s]"), *ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipmentResult));
#endif
			 }
			 return;
		}
		return;
	}
	OwnerPlayerInputManager->ServerGrabEquippedItemToCursor(SlotTag);
}

void UObInventoryItemsWidgetController::HandleRightClickingOnStashedItem(const FObsidianItemPosition& AtItemPosition,
	UObsidianItem* ItemWidget)
{
	check(OwnerPlayerInputManager);
	if(OwnerPlayerInputManager && OwnerPlayerInputManager->IsDraggingAnItem())
	{
		return;
	}
	
	check(PlayerStashComponent);
	if(PlayerStashComponent && PlayerStashComponent->CanOwnerModifyPlayerStashState() == false)
	{
		return;
	}
	
	UObsidianInventoryItemInstance* UsingInstance = PlayerStashComponent->GetItemInstanceFromTabAtPosition(AtItemPosition);
	if(UsingInstance && UsingInstance->IsItemUsable() == false)
	{
		return;
	}

	if(UsingInstance->GetUsableItemType() == EObsidianUsableItemType::UIT_Crafting)
	{
		ItemWidget = ItemWidget == nullptr ? GetItemWidgetAtStashPosition(AtItemPosition) : ItemWidget;
		if (ItemWidget == nullptr)
		{
			UE_LOG(LogWidgetController_Items, Error, TEXT("ItemWidget is invalid in [%hs]."), __FUNCTION__);
			return;
		}
		
		OwnerPlayerInputManager->SetUsingItem(true, ItemWidget, UsingInstance);

		TArray<UObsidianInventoryItemInstance*> AllItems;
		AllItems.Append(InventoryComponent->GetAllItems());
		AllItems.Append(EquipmentComponent->GetAllEquippedItems());
		AllItems.Append(PlayerStashComponent->GetAllItems()); //TODO(intrxx) This will be hella slow, change later 
	
		const FObsidianItemsMatchingUsableContext MatchingUsableContext = UsingInstance->FireItemUseUIContext(AllItems);
		for(const FIntPoint& GridLocation : MatchingUsableContext.InventoryItemsMatchingContext)
		{
			if(UObsidianItem* Item = GetItemWidgetFromInventoryAtGridPosition(GridLocation))
			{
				Item->HighlightItem();
				CachedItemsMatchingUsableContext.Add(Item);
			}
		}
		for(const FGameplayTag& SlotTag : MatchingUsableContext.EquipmentItemsMatchingContext)
		{
			//TODO(intrxx) implement hightlight
			// if(UObsidianItem* Item = GetItemWidgetAtEquipmentSlot(SlotTag))
			// {
			// 	Item->HighlightItem();
			// 	CachedItemsMatchingUsableContext.Add(Item);
			// }
		}
		for(const FObsidianItemPosition& StashPosition : MatchingUsableContext.StashItemsMatchingContext)
		{
			if (UObsidianItem* Item = GetItemWidgetAtStashPosition(StashPosition))
			{
				Item->HighlightItem();
				CachedItemsMatchingUsableContext.Add(Item);
			}
		}
	}
	else if(UsingInstance->GetUsableItemType() == EObsidianUsableItemType::UIT_Activation)
	{
		OwnerPlayerInputManager->ServerActivateUsableItemFromInventory(UsingInstance);
	}
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnStashedItem(const FObsidianItemPosition& AtItemPosition, const bool bAddToOtherWindow)
{
	check(PlayerStashComponent);
	check(OwnerPlayerInputManager);
	check(DraggedItemWidgetClass);

	if(PlayerStashComponent->CanOwnerModifyPlayerStashState() == false)
	{
		return;
	}
	
	RemoveItemUIElements();
	
	if (OwnerPlayerInputManager->IsUsingItem())
	{
		OwnerPlayerInputManager->UseItem(AtItemPosition, false);
		return;
	}

	if (OwnerPlayerInputManager->IsDraggingAnItem() == false)
	{
		if (bAddToOtherWindow == false)
		{
			OwnerPlayerInputManager->ServerGrabStashedItemToCursor(AtItemPosition);
			return;
		}

		const AObsidianHUD* ObsidianHUD = ObsidianPlayerController->GetObsidianHUD();
		if (ObsidianHUD && ObsidianHUD->IsInventoryOpened()) //TODO(intrxx) For now I support only Inventory <-> Stash
		{
			OwnerPlayerInputManager->ServerTransferItemToInventory(AtItemPosition);
		}
		return;
	}
	
	
	const UObsidianInventoryItemInstance* InstanceToAddTo = PlayerStashComponent->GetItemInstanceFromTabAtPosition(AtItemPosition);
	if (InstanceToAddTo == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Item Instance at pressed Location is invalid in [%hs]"), __FUNCTION__);
		return;
	}
		
	const FDraggedItem DraggedItem = OwnerPlayerInputManager->GetDraggedItem();
	if (const UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance) // We carry item instance.
	{
		if (DraggedInstance->IsStackable() && UObsidianItemsFunctionLibrary::IsTheSameItem(DraggedInstance, InstanceToAddTo))
		{
			OwnerPlayerInputManager->ServerAddStacksFromDraggedItemToStashedItemAtSlot(AtItemPosition);
		}
		else if (PlayerStashComponent->CanReplaceItemAtPosition(AtItemPosition, DraggedInstance))
		{
			OwnerPlayerInputManager->ServerReplaceItemAtStashPosition(AtItemPosition);
		}
	}
	else if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef) // We carry item def
	{
		const UObsidianInventoryItemDefinition* DefaultObject = DraggedItemDef.GetDefaultObject();
		if (DefaultObject && DefaultObject->IsStackable() && UObsidianItemsFunctionLibrary::IsTheSameItem_WithDef(InstanceToAddTo, DraggedItemDef))
		{
			OwnerPlayerInputManager->ServerAddStacksFromDraggedItemToStashedItemAtSlot(AtItemPosition);
		}
		else if(PlayerStashComponent->CanReplaceItemAtPosition(AtItemPosition, DraggedItemDef))
		{
			OwnerPlayerInputManager->ServerReplaceItemAtStashPosition(AtItemPosition);
		}
	}
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnStashedItemWithShiftDown(const FObsidianItemPosition& AtItemPosition, const UObsidianItem* ItemWidget)
{
	check(OwnerPlayerInputManager);
	check(PlayerStashComponent);

	if(PlayerStashComponent->CanOwnerModifyPlayerStashState() == false)
	{
		return;
	}
	
	if(OwnerPlayerInputManager->IsUsingItem())
	{
		OwnerPlayerInputManager->UseItem(AtItemPosition, true);
		return;
	}
	
	if(OwnerPlayerInputManager->IsDraggingAnItem())
	{
		OwnerPlayerInputManager->ServerAddStacksFromDraggedItemToStashedItemAtSlot(AtItemPosition, 1);
		return;
	}
	
	UObsidianInventoryItemInstance* ItemInstance = PlayerStashComponent->GetItemInstanceFromTabAtPosition(AtItemPosition);
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

	checkf(UnstackSliderClass, TEXT("Tried to create widget without valid widget class in fill it in UObInventoryItemsWidgetController instance."));
	ActiveUnstackSlider = CreateWidget<UObsidianUnstackSlider>(ObsidianPlayerController, UnstackSliderClass);
	ActiveUnstackSlider->InitializeUnstackSlider(CurrentItemStacks, AtItemPosition);

	const FVector2D UnstackSliderViewportPosition = CalculateUnstackSliderPosition(ItemWidget);
	ActiveUnstackSlider->SetPositionInViewport(UnstackSliderViewportPosition);
	ActiveUnstackSlider->AddToViewport();
	bUnstackSliderActive = true;
	
	ActiveUnstackSlider->OnAcceptButtonPressedDelegate.AddUObject(this, &ThisClass::HandleTakingOutStacksFromStash);
	ActiveUnstackSlider->OnCloseButtonPressedDelegate.AddUObject(this, &ThisClass::RemoveUnstackSlider);
}

void UObInventoryItemsWidgetController::HandleHoveringOverItem(const FObsidianItemPosition& ItemPosition,
	const FObsidianItemInteractionData& InteractionData, const EObsidianPanelOwner PanelOwner)
{
	if (CanShowDescription() == false)
	{
		return;
	}
	
	if(InteractionData.ItemWidget == nullptr || ItemPosition.IsValid() == false)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("ItemWidget or ItemPosition are invalid in [%hs]."),
			__FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* ForInstance = nullptr;
	switch (PanelOwner)
	{
		case EObsidianPanelOwner::Inventory:
			{
				ensureMsgf(ItemPosition.IsOnInventoryGrid(), TEXT("Trying to hover over Inventorized Item with"
														" invalid Inventory position."));
				
				ForInstance = InventoryComponent->GetItemInstanceAtLocation(ItemPosition.GetItemGridPosition());
				break;
			}
		case EObsidianPanelOwner::Equipment:
			{
				ensureMsgf(ItemPosition.IsOnEquipmentSlot(), TEXT("Trying to hover over Equipped Item with"
														" invalid Equipment position."));
				
				ForInstance = EquipmentComponent->GetEquippedInstanceAtSlot(ItemPosition.GetItemSlotTag());
				break;
			}
		case EObsidianPanelOwner::PlayerStash:
			{
				ensureMsgf(ItemPosition.IsOnStash(), TEXT("Trying to hover over Stashed Item with"
														" invalid Stash position."));
				
				ForInstance = PlayerStashComponent->GetItemInstanceFromTabAtPosition(ItemPosition);
				break;
			}
			default:
			{
				break;
			}
	}

	if (ForInstance == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Was unable to retrieve the Item Instance from"
			" Item Position [%s] in [%hs]."), *ItemPosition.GetDebugStringPosition(), __FUNCTION__);
		return;
	}

	FObsidianItemStats OutItemStats;
	if (UObsidianItemsFunctionLibrary::GetItemStats(ObsidianPlayerController, ForInstance, OutItemStats))
	{
		CreateInventoryItemDescription(ItemPosition, InteractionData.ItemWidget, OutItemStats);
	}
}

bool UObInventoryItemsWidgetController::CanShowDescription() const
{
	return !bUnstackSliderActive;
}

void UObInventoryItemsWidgetController::HandleUnhoveringItem(const FObsidianItemPosition& FromPosition)
{
	ClearItemDescriptionForPosition(FromPosition);
}

void UObInventoryItemsWidgetController::ClearItemDescriptionForPosition(const FObsidianItemPosition& ForPosition)
{
	UObsidianItemDescriptionBase* RemovedDescription = nullptr;
	if (ActiveItemDescriptions.RemoveAndCopyValue(ForPosition, RemovedDescription))
	{
		if (RemovedDescription)
		{
			RemovedDescription->DestroyDescriptionWidget();
		}
	}
}

void UObInventoryItemsWidgetController::CreateItemDescriptionForDroppedItem(const UObsidianInventoryItemInstance* Instance)
{
	if(CanShowDescription() == false)
	{
		return;
	}
	
	FObsidianItemStats OutItemStats;
	if(UObsidianItemsFunctionLibrary::GetItemStats(ObsidianPlayerController, Instance, OutItemStats))
	{
		CreateDroppedItemDescription(OutItemStats);
	}
}

void UObInventoryItemsWidgetController::CreateItemDescriptionForDroppedItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FObsidianItemGeneratedData& ItemGeneratedData)
{
	if(!CanShowDescription())
	{
		return;
	}
	
	FObsidianItemStats OutItemStats;
	if(UObsidianItemsFunctionLibrary::GetItemStats_WithDef(ObsidianPlayerController, ItemDef, ItemGeneratedData, OutItemStats))
	{
		CreateDroppedItemDescription(OutItemStats);
	}
}

void UObInventoryItemsWidgetController::HandleTakingOutStacksFromInventory(const int32 StacksToTake, const FObsidianItemPosition& ItemPosition)
{
	RemoveUnstackSlider();
	
	if(StacksToTake == 0 || InventoryComponent == nullptr)
	{
		return;
	}

	const FIntPoint GridPosition = ItemPosition.GetItemGridPosition();
	
	if(const UObsidianInventoryItemInstance* Instance = InventoryComponent->GetItemInstanceAtLocation(GridPosition))
	{
		if(Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current) == StacksToTake)
		{
			OwnerPlayerInputManager->ServerGrabInventoryItemToCursor(GridPosition);
			return;
		}
		OwnerPlayerInputManager->ServerTakeoutFromInventoryItem(GridPosition, StacksToTake);
	}
}

void UObInventoryItemsWidgetController::HandleTakingOutStacksFromStash(const int32 StacksToTake, const FObsidianItemPosition& ItemPosition)
{
	RemoveUnstackSlider();
	
	if(StacksToTake == 0 || PlayerStashComponent == nullptr)
	{
		return;
	}
	
	if(const UObsidianInventoryItemInstance* Instance = PlayerStashComponent->GetItemInstanceFromTabAtPosition(ItemPosition))
	{
		if(Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current) == StacksToTake)
		{
			OwnerPlayerInputManager->ServerGrabStashedItemToCursor(ItemPosition);
			return;
		}
		OwnerPlayerInputManager->ServerTakeoutFromStashedItem(ItemPosition, StacksToTake);
	}
}

void UObInventoryItemsWidgetController::RemoveItemUIElements()
{
	RemoveUnstackSlider();
	//TODO(intrxx) Remove all Active Descriptions? 
}

void UObInventoryItemsWidgetController::RemoveUnstackSlider()
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

void UObInventoryItemsWidgetController::RemoveCurrentDroppedItemDescription()
{
	if(bDroppedDescriptionActive && ActiveDroppedItemDescription)
	{
		ActiveDroppedItemDescription->DestroyDescriptionWidget();
		ActiveDroppedItemDescription = nullptr;
		bDroppedDescriptionActive = false;
	}
}

void UObInventoryItemsWidgetController::StopUsingItem()
{
	OwnerPlayerInputManager->SetUsingItem(false);
}

void UObInventoryItemsWidgetController::ClearUsableUIContext()
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

bool UObInventoryItemsWidgetController::IsDraggingAnItem() const
{
	if(OwnerPlayerInputManager)
	{
		return OwnerPlayerInputManager->IsDraggingAnItem();
	}
	return false;
}

bool UObInventoryItemsWidgetController::CanPlaceDraggedItemInInventory(const FIntPoint& AtGridSlot) const
{
	if(OwnerPlayerInputManager == nullptr || InventoryComponent == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("OwnerPlayerInputManager or InventoryComponent is"
			" invalid in [%hs]"), __FUNCTION__);
		return false;	
	}
	
	const FIntPoint DraggedItemGridSpan = GetDraggedItemGridSpan();
	if(DraggedItemGridSpan == FIntPoint::NoneValue)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Failed to retrieve Dragged Item Grid Span in [%hs]"),
			__FUNCTION__);
		return false;
	}
	
	return InventoryComponent->CheckSpecifiedPosition(DraggedItemGridSpan, AtGridSlot);
}

bool UObInventoryItemsWidgetController::CanPlaceDraggedItemInStash(const FObsidianItemPosition& ItemPosition) const
{
	if(OwnerPlayerInputManager == nullptr || PlayerStashComponent == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("OwnerPlayerInputManager or PlayerStashComponent is"
			" invalid in [%hs]"), __FUNCTION__);
		return false;	
	}
	
	const FDraggedItem DraggedItem = OwnerPlayerInputManager->GetDraggedItem();
	if (DraggedItem.IsEmpty())
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Failed to retrieve Dragged Item in  [%hs]"),
			__FUNCTION__);
		return false;
	}
	
	FGameplayTag CategoryTag;
	FGameplayTag ItemBaseType;
	const bool bSuccess = UObsidianItemsFunctionLibrary::GetItemCategoryAndBaseItemTypeTagsFromDraggedItem(DraggedItem,
		CategoryTag, ItemBaseType);
	if(bSuccess == false)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Failed to retrieve Dragged Item Category Tag and"
			" Item Base Type in [%hs]"), __FUNCTION__);
		return false;
	}

	const FIntPoint DraggedItemGridSpan = UObsidianItemsFunctionLibrary::GetGridSpanFromDraggedItem(DraggedItem);
	if(DraggedItemGridSpan == FIntPoint::NoneValue)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Failed to retrieve Dragged Item Grid Span in [%hs]"),
			__FUNCTION__);
		return false;
	}
	
	return PlayerStashComponent->CheckSpecifiedPosition(ItemPosition, CategoryTag, ItemBaseType, DraggedItemGridSpan);
}

bool UObInventoryItemsWidgetController::CanInteractWithGrid(const EObsidianPanelOwner PanelOwner) const
{
	switch(PanelOwner)
	{
	case EObsidianPanelOwner::Inventory:
		if(InventoryComponent)
		{
			return InventoryComponent->CanOwnerModifyInventoryState();
		}
		break;
	case EObsidianPanelOwner::PlayerStash:
		if(PlayerStashComponent)
		{
			return PlayerStashComponent->CanOwnerModifyPlayerStashState();
		}
		break;
	default:
		UE_LOG(LogWidgetController_Items, Error, TEXT("There is no valid PanelOwner in [%hs]"), __FUNCTION__);
		break;
	}
	return false;
}

bool UObInventoryItemsWidgetController::CanInteractWithSlots(const EObsidianPanelOwner PanelOwner) const
{
	switch(PanelOwner)
	{
	case EObsidianPanelOwner::Equipment:
		if(EquipmentComponent)
		{
			return EquipmentComponent->CanOwnerModifyEquipmentState();
		}
		break;
	case EObsidianPanelOwner::PlayerStash:
		if(PlayerStashComponent)
		{
			return PlayerStashComponent->CanOwnerModifyPlayerStashState();
		}
		break;
	default:
		UE_LOG(LogWidgetController_Items, Error, TEXT("There is no valid PanelOwner in [%hs]"), __FUNCTION__);
		break;
	}
	return false;
}

bool UObInventoryItemsWidgetController::CanInteractWithInventory() const
{
	if(InventoryComponent)
	{
		return InventoryComponent->CanOwnerModifyInventoryState();
	}
	return false;
}

bool UObInventoryItemsWidgetController::CanInteractWithPlayerStash() const
{
	if(PlayerStashComponent)
	{
		return PlayerStashComponent->CanOwnerModifyPlayerStashState();
	}
	return false;
}

bool UObInventoryItemsWidgetController::CanInteractWithEquipment() const
{
	if(EquipmentComponent)
	{
		return EquipmentComponent->CanOwnerModifyEquipmentState();
	}
	return false;
}

FIntPoint UObInventoryItemsWidgetController::GetDraggedItemGridSpan() const
{
	if(!IsDraggingAnItem())
	{
		return FIntPoint::NoneValue;
	}
	
	const FDraggedItem DraggedItem = OwnerPlayerInputManager->GetDraggedItem();
	if(DraggedItem.IsEmpty())
	{
		return FIntPoint::NoneValue;
	}
	
	if(const UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		return Instance->GetItemGridSpan();
	}
	
	if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
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

FIntPoint UObInventoryItemsWidgetController::GetItemGridSpanByPosition(const FObsidianItemPosition& ItemPosition) const
{
	if (ItemPosition.IsValid() == false)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Trying to retrieve Item Grid Span from invalid Position."));
		return FIntPoint::NoneValue; 
	}

	if (ItemPosition.IsOnInventoryGrid())
	{
		if (const UObsidianInventoryItemInstance* Instance = InventoryComponent->GetItemInstanceAtLocation(
			ItemPosition.GetItemGridPosition()))
		{
			return Instance->GetItemGridSpan();
		}
	}
	else if (ItemPosition.IsOnStash())
	{
		if (const UObsidianInventoryItemInstance* Instance = PlayerStashComponent->GetItemInstanceFromTabAtPosition(
			ItemPosition))
		{
			return Instance->GetItemGridSpan();
		}
	}
	else if (ItemPosition.IsOnEquipmentSlot())
	{
		if (const UObsidianInventoryItemInstance* Instance = EquipmentComponent->GetEquippedInstanceAtSlot(
			ItemPosition.GetItemSlotTag()))
		{
			return Instance->GetItemGridSpan();
		}
	}
	
	UE_LOG(LogWidgetController_Items, Error, TEXT("Was unable to retrieve Item Grid Span from invalid Position."));
	return FIntPoint::NoneValue;
}

UObsidianItem* UObInventoryItemsWidgetController::GetItemWidgetAtItemPosition(
	const FObsidianItemPosition& AtItemPosition) const
{
	return nullptr; //TODO(intrxx) implement
}

UObsidianItem* UObInventoryItemsWidgetController::GetItemWidgetFromEquipmentPanelAtSlot(
	const FObsidianItemPosition& AtItemPosition) const
{
	return nullptr; //TODO(intrxx) implement
}

UObsidianItem* UObInventoryItemsWidgetController::GetItemWidgetFromInventoryAtGridPosition(
	const FObsidianItemPosition& AtGridSlot) const
{
	return nullptr; //TODO(intrxx) implement
}

UObsidianItem* UObInventoryItemsWidgetController::GetItemWidgetAtStashPosition(
	const FObsidianItemPosition& ItemPosition) const
{
	return nullptr; //TODO(intrxx) implement
}

FString UObInventoryItemsWidgetController::GetStashTabName(const FGameplayTag StashTabTag) const
{
	if (PlayerStashComponent)
	{
		if (const UObsidianStashTab* StashTab = PlayerStashComponent->GetStashTabForTag(StashTabTag))
		{
			return StashTab->GetStashTabName();
		}
	}
	return FString();
}

void UObInventoryItemsWidgetController::RegisterCurrentStashTab(const FGameplayTag& CurrentStashTab)
{
	if (PlayerStashComponent)
	{
		PlayerStashComponent->ServerRegisterAndValidateCurrentStashTab(CurrentStashTab);
	}
}

bool UObInventoryItemsWidgetController::CanPlaceDraggedItemAtPosition(const FObsidianItemPosition& AtPosition,
	const EObsidianPanelOwner PanelOwner) const
{
	if (IsDraggingAnItem() == false)
	{
		return false;
	}
	
	check((uint8)PanelOwner > 0);
	switch (PanelOwner)
	{
	case EObsidianPanelOwner::Inventory:
		{
			ensureMsgf(AtPosition.IsOnInventoryGrid(), TEXT("Trying to add item to Inventory with"
													" invalid Inventory position."));
			
			return CanPlaceDraggedItemInInventory(AtPosition.GetItemGridPosition());
		}
	case EObsidianPanelOwner::Equipment:
		{
			ensureMsgf(AtPosition.IsOnEquipmentSlot(), TEXT("Trying to add item to Equipment with"
													" invalid Equipment position."));
				
			return CanPlaceDraggedItemInEquipment(AtPosition.GetItemSlotTag());
		}
	case EObsidianPanelOwner::PlayerStash:
		{
			ensureMsgf(AtPosition.IsOnStash(), TEXT("Trying to add item to Stash with"
													" invalid Stash position."));

			return CanPlaceDraggedItemInStash(AtPosition);
		}
	default:
		{
			UE_LOG(LogWidgetController_Items, Error, TEXT("[%d] PanelOwner is invalid in [%hs]."),
				PanelOwner, __FUNCTION__);
			return false;
		}
	}
}

bool UObInventoryItemsWidgetController::CanPlaceDraggedItemInEquipment(const FGameplayTag& SlotTag) const
{
	if(OwnerPlayerInputManager == nullptr || EquipmentComponent == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("OwnerPlayerInputManager or EquipmentComponent is invalid in [%hs]."),
			__FUNCTION__);
		return false; 
	}

	const bool bSlotOccupied = EquipmentComponent->IsItemEquippedAtSlot(SlotTag);
	const FDraggedItem DraggedItem = OwnerPlayerInputManager->GetDraggedItem();
	if(const UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance)
	{
		EObsidianEquipCheckResult EquipResult;
		if(bSlotOccupied)
		{
			EquipResult	= EquipmentComponent->CanReplaceInstance(DraggedInstance, SlotTag);
		}
		else
		{
			EquipResult	= EquipmentComponent->CanEquipInstance(DraggedInstance, SlotTag);
		}
		 
		return EquipResult == EObsidianEquipCheckResult::CanEquip;
	}
	if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef)
	{
		EObsidianEquipCheckResult EquipResult;
		if(bSlotOccupied)
		{
			EquipResult = EquipmentComponent->CanReplaceTemplate(DraggedItemDef, SlotTag, DraggedItem.GeneratedData);
		}
		else
		{
			EquipResult = EquipmentComponent->CanEquipTemplate(DraggedItemDef, SlotTag, DraggedItem.GeneratedData);
		}
		
		return EquipResult == EObsidianEquipCheckResult::CanEquip;
	}
	return false;
}

UObsidianItemDescriptionBase* UObInventoryItemsWidgetController::CreateInventoryItemDescription(const FObsidianItemPosition& AtPosition,
	const UObsidianItem* ForItemWidget, const FObsidianItemStats& ItemStats)
{
	ClearItemDescriptionForPosition(AtPosition); //TODO(intrxx) will it be necessary?
	
	checkf(ItemDescriptionClass, TEXT("Tried to create widget without valid widget class, fill it in "
								   "UObInventoryItemsWidgetController instance."));
	UObsidianItemDescriptionBase* NewItemDescription = CreateWidget<UObsidianItemDescriptionBase>(ObsidianPlayerController,
		ItemDescriptionClass);
	NewItemDescription->InitializeWidgetWithItemStats(ItemStats);
	NewItemDescription->AddToViewport();
	
	const FVector2D DescriptionViewportPosition = CalculateDescriptionPosition(ForItemWidget, NewItemDescription);
	NewItemDescription->SetPositionInViewport(DescriptionViewportPosition);

	ActiveItemDescriptions.Add(AtPosition, NewItemDescription);
	
	return NewItemDescription;
}

UObsidianItemDescriptionBase* UObInventoryItemsWidgetController::CreateDroppedItemDescription(const FObsidianItemStats& ItemStats)
{
	check(ObsidianPlayerController);
	
	AObsidianHUD* ObsidianHUD = ObsidianPlayerController->GetObsidianHUD();
	if(ObsidianHUD == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Unable to get ObsidianHUD in UObInventoryItemsWidgetController::CreateItemDescriptionForDroppedItem."));
		return nullptr;
	}

	const UObsidianMainOverlay* MainOverlay = ObsidianHUD->GetMainOverlay();
	if(MainOverlay == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Unable to get ObsidianMainOverlay in UObInventoryItemsWidgetController::CreateItemDescriptionForDroppedItem."));
		return nullptr;
	}

	RemoveCurrentDroppedItemDescription(); // Clear any other Item Description

	checkf(ItemDescriptionClass, TEXT("Tried to create widget without valid widget class, fill it in UObInventoryItemsWidgetController instance."));
	ActiveDroppedItemDescription = CreateWidget<UObsidianItemDescriptionBase>(ObsidianPlayerController, ItemDescriptionClass);
	ActiveDroppedItemDescription->InitializeWidgetWithItemStats(ItemStats, true);
	MainOverlay->AddItemDescriptionToOverlay(ActiveDroppedItemDescription);
	bDroppedDescriptionActive = true;
	
	return ActiveDroppedItemDescription;
}

FVector2D UObInventoryItemsWidgetController::CalculateUnstackSliderPosition(const UObsidianItem* ItemWidget) const
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Failed to calculate Unstack Slider Position"));
		return FVector2D::Zero();
	}

	if(ItemWidget == nullptr || ActiveUnstackSlider == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Failed to calculate Unstack Slider Position"));
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

FVector2D UObInventoryItemsWidgetController::CalculateDescriptionPosition(const UObsidianItem* ItemWidget,
	UObsidianItemDescriptionBase* ForDescription) const
{
	const UWorld* World = GetWorld();
	if(World == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Failed to calculate Description Position"));
		return FVector2D::Zero();
	}

	if(ItemWidget == nullptr || ForDescription == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Failed to calculate Description Position"));
		return FVector2D::Zero();
	}

	// @HACK this is quite ugly, but without prepass the desired size is [0, 0], if the performance is the problem,
	// I could delay the calculation for a frame and see how reliable it is to retrieve the sie information,
	// Other system with delegates could be implemented to get the size reliably, but it just needs testing cuz if it's not bad I don't really care for now.
	ForDescription->ForceLayoutPrepass();
	FVector2D DescriptionSize = ForDescription->GetDesiredSize();
		
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

FVector2D UObInventoryItemsWidgetController::GetItemUIElementPositionBoundByViewport(const FVector2D& ViewportSize, const FVector2D& ItemPosition, const FVector2D& ItemSize, const FVector2D& UIElementSize) const
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



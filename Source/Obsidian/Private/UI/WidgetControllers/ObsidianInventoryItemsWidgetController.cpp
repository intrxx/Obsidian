// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/WidgetControllers/ObsidianInventoryItemsWidgetController.h"

// ~ Core
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"

// ~ Project
#include "UI/Inventory/Items/ObsidianItemDescriptionBase.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "CharacterComponents/ObsidianPlayerInputManager.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Core/ObsidianGameplayStatics.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/ObsidianHUD.h"
#include "InventoryItems/Equipment/ObsidianEquipmentList.h"
#include "InventoryItems/PlayerStash/ObsidianStashTabsConfig.h"
#include "UI/Inventory/Items/ObsidianDraggedItem.h"
#include "UI/Inventory/Items/ObsidianItem.h"
#include "UI/Inventory/Slots/ObsidianSlotBlockadeItem.h"
#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot_Equipment.h"
#include "Obsidian/Public/UI/Inventory/Items/ObsidianUnstackSlider.h"
#include "UI/MainOverlay/ObsidianMainOverlay.h"

DEFINE_LOG_CATEGORY(LogWidgetController_Items);

// ~ Start of FObsidianItemWidgetData

bool FObsidianItemWidgetData::IsItemForSwapSlot() const 
{
	const FGameplayTag DesiredSlot = ItemPosition.GetItemSlotTag();
	if(DesiredSlot.IsValid() && DesiredSlot.MatchesTag(FGameplayTag::RequestGameplayTag("Item.SwapSlot.Equipment", true)))
	{
		return true;
	}
	return false;
}

// ~ Start of FObsidianStashAddedItemWidgets

FGameplayTag FObsidianStashAddedItemWidgets::GetStashTag() const
{
	return StashTag;
}

int32 FObsidianStashAddedItemWidgets::GetNumberOfItemsAdded() const
{
	return StashAddedItemWidgetsMap.Num();
}

UObsidianItem* FObsidianStashAddedItemWidgets::GetItemWidgetAtPosition(const FObsidianItemPosition& ItemPosition) const
{
	return StashAddedItemWidgetsMap.FindRef(ItemPosition);
}

bool FObsidianStashAddedItemWidgets::AddItemWidget(const FObsidianItemPosition& ItemPosition, UObsidianItem* ItemWidget)
{
	if(!StashAddedItemWidgetsMap.Contains(ItemPosition))
	{
		StashAddedItemWidgetsMap.Add(ItemPosition, ItemWidget);
		return true;
	}
#if !UE_BUILD_SHIPPING
	FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Widget at provided ItemPosition already exists in provided Stash Tab with tag [%s]"),
		*StashTag.GetTagName().ToString()), ELogVerbosity::Error);
#endif
	return false;
}

bool FObsidianStashAddedItemWidgets::RemoveItemWidget(const FObsidianItemPosition& ItemPosition)
{
	if (UObsidianItem** ItemPointer = StashAddedItemWidgetsMap.Find(ItemPosition))
	{
		UObsidianItem* ItemWidget = *ItemPointer;
		ItemWidget->RemoveFromParent();

		StashAddedItemWidgetsMap.Remove(ItemPosition);
		return true;
	}
	return false;
}

void FObsidianStashAddedItemWidgets::EmptyAddedItemWidgets(const int32 OptionalReserve)
{
	StashAddedItemWidgetsMap.Empty(OptionalReserve);
}

void FObsidianStashAddedItemWidgets::DebugPrintAllAddedItems()
{
	const FString StashTagString = *StashTag.GetTagName().ToString();
	UE_LOG(LogWidgetController_Items, Warning, TEXT("------------------ Printing All Added Widgets from Stash Tab [%s] ------------------"), *StashTagString);
	
	for (const TPair<FObsidianItemPosition, UObsidianItem*>& Pair : StashAddedItemWidgetsMap)
	{
		FIntPoint GridLocation = Pair.Key.GetItemGridLocation(false);
		FGameplayTag SlotTag = Pair.Key.GetItemSlotTag(false);
		if (GridLocation != FIntPoint::NoneValue)
		{
			UE_LOG(LogWidgetController_Items, Display, TEXT("Item Widget [%s] at position [%d, %d]"), *GetNameSafe(Pair.Value), GridLocation.X, GridLocation.Y);
		}
		else
		{
			UE_LOG(LogWidgetController_Items, Display, TEXT("Item Widget [%s] at slot [%s]"), *GetNameSafe(Pair.Value), *SlotTag.GetTagName().ToString());
		}
	}
	
	UE_LOG(LogWidgetController_Items, Warning, TEXT("------------------ End of Stash [%s] ------------------"), *StashTagString);
}

// ~ End of FObsidianStashAddedItemWidgets

void UObsidianInventoryItemsWidgetController::OnWidgetControllerSetupCompleted()
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

	RegisterInitialStashTabs();
}

void UObsidianInventoryItemsWidgetController::OnInventoryStateChanged(FGameplayTag Channel, const FObsidianInventoryChangeMessage& InventoryChangeMessage)
{
	if(InventoryComponent != InventoryChangeMessage.InventoryOwner) // Fixes a bug when Items appear in Server's Inventory (Listen Server Character) after picked up by client.
	{
		return;
	}
	
	const UObsidianInventoryItemInstance* Instance = InventoryChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Item Instance is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
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
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Adding item: [%s] to Inventory"), *Instance->GetItemDisplayName().ToString());
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemImage = Instance->GetItemImage();
		ItemWidgetData.ItemPosition = InventoryChangeMessage.GridItemPosition;
		ItemWidgetData.GridSpan = Instance->GetItemGridSpan();
		ItemWidgetData.StackCount = Instance->IsStackable() ? InventoryChangeMessage.NewCount : 0;
		ItemWidgetData.bUsable = Instance->IsItemUsable();
		ItemWidgetData.ItemSlotPadding = Instance->GetItemSlotPadding();
		
		OnItemAddedDelegate.Broadcast(ItemWidgetData);
	}
	else if(InventoryChangeMessage.ChangeType == EObsidianInventoryChangeType::ICT_ItemRemoved)
	{
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Removing item: [%s] from Inventory"), *Instance->GetItemDisplayName().ToString());
		RemoveInventoryItemWidget(InventoryChangeMessage.GridItemPosition);
		
		if(OwnerPlayerInputManager->IsUsingItem())
		{
			if(InventoryChangeMessage.ItemInstance == OwnerPlayerInputManager->GetUsingItem())
			{
				OwnerPlayerInputManager->SetUsingItem(false);  //TODO(intrxx) This probably shouldn't be here, its widget controller
			}
		}
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

void UObsidianInventoryItemsWidgetController::OnEquipmentStateChanged(FGameplayTag Channel, const FObsidianEquipmentChangeMessage& EquipmentChangeMessage)
{
	if(EquipmentComponent != EquipmentChangeMessage.EquipmentOwner) // Fixes a bug when Items appear in Server's Inventory (Listen Server Character) after picked up by client.
	{
		return;
	}
	
	const UObsidianInventoryItemInstance* Instance = EquipmentChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Inventory Item Instance is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
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
			RemoveEquipmentItemWidget(SlotTagToClear);
		}

		if(Instance->DoesItemNeedTwoSlots())
		{
			RemoveBlockedSlotItemWidget(SlotTagToClear);
		}
	}
	else if(EquipmentChangeMessage.ChangeType == EObsidianEquipmentChangeType::ECT_ItemSwapped)
	{
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Equipment Swapping item: [%s]"), *Instance->GetItemDisplayName().ToString());
		
		const FGameplayTag SlotTagToClear = EquipmentChangeMessage.SlotTagToClear;
		if(SlotTagToClear.IsValid())
		{
			RemoveEquipmentItemWidget(SlotTagToClear);
		}

		if(Instance->DoesItemNeedTwoSlots())
		{
			const FGameplayTag EquipmentTag = SlotTagToClear == FGameplayTag::EmptyTag ? UObsidianGameplayStatics::GetOpposedEuipmentTagForTag(EquipmentChangeMessage.SlotTag) : EquipmentChangeMessage.SlotTagToClear;
			RemoveBlockedSlotItemWidget(EquipmentTag);
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

void UObsidianInventoryItemsWidgetController::OnPlayerStashChanged(FGameplayTag Channel, const FObsidianStashChangeMessage& StashChangeMessage)
{
	if(PlayerStashComponent != StashChangeMessage.PlayerStashOwner) // Fixes a bug when Items appear in Server's Inventory (Listen Server Character) after picked up by client.
	{
		return;
	}
	
	const UObsidianInventoryItemInstance* Instance = StashChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Item Instance is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	if(StashChangeMessage.ChangeType == EObsidianStashChangeType::ICT_GeneralItemChanged)
	{
		// HandleHoveringOverInventoryItem(StashChangeMessage.GridItemPosition);
		// if(UObsidianItem* CorrespondingItemWidget = GetItemWidgetAtInventoryGridSlot(InventoryChangeMessage.GridItemPosition))
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
		RemoveStashItemWidget(StashChangeMessage.ItemPosition);
		
		if(OwnerPlayerInputManager->IsUsingItem())
		{
			if(StashChangeMessage.ItemInstance == OwnerPlayerInputManager->GetUsingItem())
			{
				OwnerPlayerInputManager->SetUsingItem(false);  //TODO(intrxx) This probably shouldn't be here, its widget controller
			}
		}
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

void UObsidianInventoryItemsWidgetController::OnStartDraggingItem(const FDraggedItem& DraggedItem)
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

void UObsidianInventoryItemsWidgetController::OnStopDraggingItem()
{
	OnStopPlacementHighlightDelegate.Broadcast();
}

int32 UObsidianInventoryItemsWidgetController::GetInventoryGridWidth() const
{
	if(InventoryComponent)
	{
		return InventoryComponent->GetInventoryGridWidth();
	}
	
	UE_LOG(LogWidgetController_Items, Error, TEXT("Trying to return Grid Width but Inventory Component is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
	return 0;
}

int32 UObsidianInventoryItemsWidgetController::GetInventoryGridHeight() const
{
	if(InventoryComponent)
	{
		return InventoryComponent->GetInventoryGridHeight();
	}
	
	UE_LOG(LogWidgetController_Items, Error, TEXT("Trying to return Grid Height but Inventory Component is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
	return 0;
}

UObsidianStashTabsConfig* UObsidianInventoryItemsWidgetController::GetStashTabConfig() const
{
	if(PlayerStashComponent)
	{
		return PlayerStashComponent->GetStashTabConfig();
	}
	
	UE_LOG(LogWidgetController_Items, Error, TEXT("Trying to return Stash Config but Player Stash Component is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
	return nullptr;
}

void UObsidianInventoryItemsWidgetController::OnInventoryOpen()
{
	TArray<UObsidianInventoryItemInstance*> InventoryItems = InventoryComponent->GetAllItems();
	AddedItemWidgetMap.Empty(InventoryItems.Num());
	
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

			OnItemAddedDelegate.Broadcast(ItemWidgetData);
		}
	}

	TArray<UObsidianInventoryItemInstance*> EquippedItems = EquipmentComponent->GetAllEquippedItems();
	EquippedItemWidgetMap.Empty(EquippedItems.Num());
	BlockedSlotsWidgetMap.Empty();

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

void UObsidianInventoryItemsWidgetController::OnPlayerStashOpen()
{
	UE_LOG(LogTemp, Display, TEXT("Stash Opened."));

	//TODO(intrxx) This for sure will need to be changed, it will be to heavy on performance.
	TArray<UObsidianInventoryItemInstance*> StashedItems = PlayerStashComponent->GetAllItems();
	
	/*
	 *TODO I need to rethink that, I might not need to reset these items since the Player Stash state
	 *	might not change without Player seeing it (but what about Stash Tab Affinities?).
	 **/
	EmptyRegisteredItems();
	
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

void UObsidianInventoryItemsWidgetController::RequestAddingItemToInventory(const FIntPoint& ToGridSlot, const bool bShiftDown)
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

void UObsidianInventoryItemsWidgetController::RequestEquippingItem(const FGameplayTag& SlotTag)
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

void UObsidianInventoryItemsWidgetController::RequestAddingItemToStashTab(const FObsidianItemPosition& ToPosition, const bool bShiftDown)
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

void UObsidianInventoryItemsWidgetController::HandleRightClickingOnInventoryItem(const FIntPoint& AtGridSlot, UObsidianItem* ItemWidget)
{
	check(OwnerPlayerInputManager);
	check(InventoryComponent);

	if(OwnerPlayerInputManager->IsDraggingAnItem())
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

	if(UsingInstance->GetUsableItemType() == EObsidianUsableItemType::UIT_Crafting)
	{
		OwnerPlayerInputManager->SetUsingItem(true, ItemWidget, UsingInstance);

		TArray<UObsidianInventoryItemInstance*> AllItems;
		AllItems.Append(InventoryComponent->GetAllItems());
		AllItems.Append(EquipmentComponent->GetAllEquippedItems());
		AllItems.Append(PlayerStashComponent->GetAllItems()); //TODO(intrxx) This will be hella slow, change later 
	
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

void UObsidianInventoryItemsWidgetController::HandleLeftClickingOnInventoryItem(const FIntPoint& AtGridSlot, const bool bAddToOtherWindow)
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
		UE_LOG(LogWidgetController_Items, Error, TEXT("Item Instance at pressed Location is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
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

void UObsidianInventoryItemsWidgetController::HandleLeftClickingOnInventoryItemWithShiftDown(const FIntPoint& AtGridSlot, const UObsidianItem* ItemWidget)
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

	checkf(UnstackSliderClass, TEXT("Tried to create widget without valid widget class in fill it in UObsidianInventoryItemsWidgetController instance."));
	ActiveUnstackSlider = CreateWidget<UObsidianUnstackSlider>(ObsidianPlayerController, UnstackSliderClass);
	ActiveUnstackSlider->InitializeUnstackSlider(CurrentItemStacks, AtGridSlot);

	const FVector2D UnstackSliderViewportPosition = CalculateUnstackSliderPosition(ItemWidget);
	ActiveUnstackSlider->SetPositionInViewport(UnstackSliderViewportPosition);
	ActiveUnstackSlider->AddToViewport();
	bUnstackSliderActive = true;
	
	ActiveUnstackSlider->OnAcceptButtonPressedDelegate.AddUObject(this, &ThisClass::HandleTakingOutStacksFromInventory);
	ActiveUnstackSlider->OnCloseButtonPressedDelegate.AddUObject(this, &ThisClass::RemoveUnstackSlider);
}

void UObsidianInventoryItemsWidgetController::HandleLeftClickingOnEquipmentItem(const FGameplayTag& SlotTag, const FGameplayTag& EquipSlotTagOverride)
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

void UObsidianInventoryItemsWidgetController::HandleRightClickingOnStashedItem(const FObsidianItemPosition& AtItemPosition, UObsidianItem* ItemWidget)
{
	check(OwnerPlayerInputManager);
	check(PlayerStashComponent);
	
	if(OwnerPlayerInputManager->IsDraggingAnItem())
	{
		return;
	}

	if(PlayerStashComponent->CanOwnerModifyPlayerStashState() == false)
	{
		return;
	}
	
	UObsidianInventoryItemInstance* UsingInstance = PlayerStashComponent->GetItemInstanceFromTabAtPosition(AtItemPosition);
	if(UsingInstance->IsItemUsable() == false)
	{
		return;
	}

	if(UsingInstance->GetUsableItemType() == EObsidianUsableItemType::UIT_Crafting)
	{
		OwnerPlayerInputManager->SetUsingItem(true, ItemWidget, UsingInstance);

		TArray<UObsidianInventoryItemInstance*> AllItems;
		AllItems.Append(InventoryComponent->GetAllItems());
		AllItems.Append(EquipmentComponent->GetAllEquippedItems());
		AllItems.Append(PlayerStashComponent->GetAllItems()); //TODO(intrxx) This will be hella slow, change later 
	
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

void UObsidianInventoryItemsWidgetController::HandleLeftClickingOnStashedItem(const FObsidianItemPosition& AtItemPosition, const bool bAddToOtherWindow)
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
		UE_LOG(LogWidgetController_Items, Error, TEXT("Item Instance at pressed Location is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
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

void UObsidianInventoryItemsWidgetController::HandleLeftClickingOnStashedItemWithShiftDown(const FObsidianItemPosition& AtItemPosition, const UObsidianItem* ItemWidget)
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

	checkf(UnstackSliderClass, TEXT("Tried to create widget without valid widget class in fill it in UObsidianInventoryItemsWidgetController instance."));
	ActiveUnstackSlider = CreateWidget<UObsidianUnstackSlider>(ObsidianPlayerController, UnstackSliderClass);
	ActiveUnstackSlider->InitializeUnstackSlider(CurrentItemStacks, AtItemPosition);

	const FVector2D UnstackSliderViewportPosition = CalculateUnstackSliderPosition(ItemWidget);
	ActiveUnstackSlider->SetPositionInViewport(UnstackSliderViewportPosition);
	ActiveUnstackSlider->AddToViewport();
	bUnstackSliderActive = true;
	
	ActiveUnstackSlider->OnAcceptButtonPressedDelegate.AddUObject(this, &ThisClass::HandleTakingOutStacksFromStash);
	ActiveUnstackSlider->OnCloseButtonPressedDelegate.AddUObject(this, &ThisClass::RemoveUnstackSlider);
}

void UObsidianInventoryItemsWidgetController::HandleHoveringOverInventoryItem(const FIntPoint& AtGridSlot)
{
	const UObsidianItem* ItemWidget = GetItemWidgetAtInventoryGridSlot(AtGridSlot);
	if(ItemWidget == nullptr || !CanShowDescription())
	{
		return;
	}
	
	const FIntPoint SlotPosition = ItemWidget->GetGridPosition();
	const UObsidianInventoryItemInstance* ItemInstance = InventoryComponent->GetItemInstanceAtLocation(AtGridSlot);

	FObsidianItemStats OutItemStats;
	const bool bSuccess = UObsidianItemsFunctionLibrary::GetItemStats(ItemInstance, OutItemStats);
	
	if(bSuccess && CreateInventoryItemDescription(ItemWidget, OutItemStats))
	{
		ActiveItemDescription->SetAssociatedItemPosition(SlotPosition);
	}
}

void UObsidianInventoryItemsWidgetController::HandleHoveringOverInventoryItem(const UObsidianItem* ItemWidget)
{
	if(ItemWidget == nullptr || !CanShowDescription())
	{
		return;
	}

	const FIntPoint SlotPosition = ItemWidget->GetGridPosition();
	const UObsidianInventoryItemInstance* ItemInstance = InventoryComponent->GetItemInstanceAtLocation(SlotPosition);

	FObsidianItemStats OutItemStats;
	const bool bSuccess = UObsidianItemsFunctionLibrary::GetItemStats(ItemInstance, OutItemStats);

	if(bSuccess && CreateInventoryItemDescription(ItemWidget, OutItemStats))
	{
		ActiveItemDescription->SetAssociatedItemPosition(SlotPosition);
	}
}

void UObsidianInventoryItemsWidgetController::HandleHoveringOverEquipmentItem(const UObsidianItem* ItemWidget)
{
	if(ItemWidget == nullptr || !CanShowDescription())
	{
		return;
	}
	
	const FGameplayTag SlotTag = ItemWidget->GetSlotTag();
	const UObsidianInventoryItemInstance* ItemInstance = EquipmentComponent->GetEquippedInstanceAtSlot(SlotTag);

	FObsidianItemStats OutItemStats;
	const bool bSuccess = UObsidianItemsFunctionLibrary::GetItemStats(ItemInstance, OutItemStats);

	if(bSuccess && CreateInventoryItemDescription(ItemWidget, OutItemStats))
	{
		ActiveItemDescription->SetAssociatedItemPosition(SlotTag);
	}
}

void UObsidianInventoryItemsWidgetController::HandleHoveringOverStashedItem(const UObsidianItem* ItemWidget)
{
	if(ItemWidget == nullptr || !CanShowDescription())
	{
		return;
	}
	
	const FObsidianItemPosition ItemPosition = ItemWidget->GetItemPosition();
	if (const UObsidianInventoryItemInstance* ItemInstance = PlayerStashComponent->GetItemInstanceFromTabAtPosition(ItemPosition))
	{
		FObsidianItemStats OutItemStats;
		const bool bSuccess = UObsidianItemsFunctionLibrary::GetItemStats(ItemInstance, OutItemStats);

		if(bSuccess && CreateInventoryItemDescription(ItemWidget, OutItemStats))
		{
			ActiveItemDescription->SetAssociatedItemPosition(ItemPosition);
		}
	}
}

bool UObsidianInventoryItemsWidgetController::CanShowDescription() const
{
	return !bUnstackSliderActive;
}

void UObsidianInventoryItemsWidgetController::HandleUnhoveringItem()
{
	RemoveCurrentItemDescription();
}

void UObsidianInventoryItemsWidgetController::CreateItemDescriptionForDroppedItem(const UObsidianInventoryItemInstance* Instance)
{
	if(CanShowDescription() == false)
	{
		return;
	}
	
	FObsidianItemStats OutItemStats;
	if(UObsidianItemsFunctionLibrary::GetItemStats(Instance, OutItemStats))
	{
		CreateDroppedItemDescription(OutItemStats);
	}
}

void UObsidianInventoryItemsWidgetController::CreateItemDescriptionForDroppedItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FObsidianItemGeneratedData& ItemGeneratedData)
{
	if(!CanShowDescription())
	{
		return;
	}
	
	FObsidianItemStats OutItemStats;
	if(UObsidianItemsFunctionLibrary::GetItemStats_WithDef(ItemDef, ItemGeneratedData, OutItemStats))
	{
		CreateDroppedItemDescription(OutItemStats);
	}
}

void UObsidianInventoryItemsWidgetController::HandleTakingOutStacksFromInventory(const int32 StacksToTake, const FObsidianItemPosition& ItemPosition)
{
	RemoveUnstackSlider();
	
	if(StacksToTake == 0 || InventoryComponent == nullptr)
	{
		return;
	}

	const FIntPoint GridPosition = ItemPosition.GetItemGridLocation();
	
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

void UObsidianInventoryItemsWidgetController::HandleTakingOutStacksFromStash(const int32 StacksToTake, const FObsidianItemPosition& ItemPosition)
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

void UObsidianInventoryItemsWidgetController::RemoveItemUIElements()
{
	RemoveUnstackSlider();
	RemoveCurrentItemDescription();
}

void UObsidianInventoryItemsWidgetController::RemoveUnstackSlider()
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

void UObsidianInventoryItemsWidgetController::RemoveCurrentItemDescription()
{
	if(bDescriptionActive && ActiveItemDescription)
	{
		ActiveItemDescription->DestroyDescriptionWidget();
		ActiveItemDescription = nullptr;
		bDescriptionActive = false;
	}
}

void UObsidianInventoryItemsWidgetController::StopUsingItem()
{
	OwnerPlayerInputManager->SetUsingItem(false);
}

void UObsidianInventoryItemsWidgetController::ClearUsableUIContext()
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

bool UObsidianInventoryItemsWidgetController::IsDraggingAnItem() const
{
	if(OwnerPlayerInputManager)
	{
		return OwnerPlayerInputManager->IsDraggingAnItem();
	}
	return false;
}

bool UObsidianInventoryItemsWidgetController::CanPlaceDraggedItem(const EObsidianGridOwner GridOwner, const FIntPoint& AtGridSlot, const FGameplayTag& StashTag) const
{
	FIntPoint LocalItemGridSpan;
	if(!GetDraggedItemGridSpan(LocalItemGridSpan))
	{
		return false;
	}
	
	switch(GridOwner)
	{
		case EObsidianGridOwner::Inventory:
			if(InventoryComponent == nullptr)
			{
				UE_LOG(LogWidgetController_Items, Error, TEXT("InventoryComponent is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
				return false;	
			}
			return InventoryComponent->CheckSpecifiedPosition(LocalItemGridSpan, AtGridSlot);
		case EObsidianGridOwner::PlayerStash:
			break;
	default:
		UE_LOG(LogWidgetController_Items, Error, TEXT("There is no valid GridOwner in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
			break;
	}
	
	return false;
}

bool UObsidianInventoryItemsWidgetController::CanPlaceDraggedItem(const EObsidianGridOwner GridOwner, const FIntPoint& AtGridSlot, const FIntPoint& ItemGridSpan, const FGameplayTag& StashTag) const
{
	switch(GridOwner)
	{
	case EObsidianGridOwner::Inventory:
		{
			if(InventoryComponent == nullptr)
			{
				UE_LOG(LogWidgetController_Items, Error, TEXT("InventoryComponent is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
				return false;	
			}
			return InventoryComponent->CheckSpecifiedPosition(ItemGridSpan,AtGridSlot);
		}
	case EObsidianGridOwner::PlayerStash:
		{
			if(OwnerPlayerInputManager == nullptr || PlayerStashComponent == nullptr)
			{
				UE_LOG(LogWidgetController_Items, Error, TEXT("OwnerPlayerInputManager or PlayerStashComponent is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
				return false;	
			}

			FGameplayTag CategoryTag;
			FGameplayTag ItemBaseType;
			UObsidianItemsFunctionLibrary::GetItemCategoryAndBaseItemTypeTagsFromDraggedItem(OwnerPlayerInputManager->GetDraggedItem(), CategoryTag, ItemBaseType);
			return PlayerStashComponent->CheckSpecifiedPosition(FObsidianItemPosition(AtGridSlot, StashTag), CategoryTag, ItemBaseType, ItemGridSpan);
		}
	default:
		{
			UE_LOG(LogWidgetController_Items, Error, TEXT("There is no valid GridOwner in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		} break;
	}
	
	return false;
}

bool UObsidianInventoryItemsWidgetController::CanPlaceItemAtStashSlot(const FObsidianItemPosition& ItemPosition) const
{
	if(OwnerPlayerInputManager == nullptr || PlayerStashComponent == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("OwnerPlayerInputManager or PlayerStashComponent is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		return false;	
	}
	
	FGameplayTag CategoryTag;
	FGameplayTag ItemBaseType;
	UObsidianItemsFunctionLibrary::GetItemCategoryAndBaseItemTypeTagsFromDraggedItem(OwnerPlayerInputManager->GetDraggedItem(), CategoryTag, ItemBaseType);
	return PlayerStashComponent->CheckSpecifiedPosition(ItemPosition, CategoryTag, ItemBaseType, FIntPoint::NoneValue);
}

bool UObsidianInventoryItemsWidgetController::CanInteractWithGrid(const EObsidianGridOwner GridOwner) const
{
	switch(GridOwner)
	{
	case EObsidianGridOwner::Inventory:
		if(InventoryComponent)
		{
			return InventoryComponent->CanOwnerModifyInventoryState();
		}
		break;
	case EObsidianGridOwner::PlayerStash:
		if(PlayerStashComponent)
		{
			return PlayerStashComponent->CanOwnerModifyPlayerStashState();
		}
		break;
	default:
		UE_LOG(LogWidgetController_Items, Error, TEXT("There is no valid GridOwner in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		break;
	}
	return false;
}

bool UObsidianInventoryItemsWidgetController::CanInteractWithInventory() const
{
	if(InventoryComponent)
	{
		return InventoryComponent->CanOwnerModifyInventoryState();
	}
	return false;
}

bool UObsidianInventoryItemsWidgetController::CanInteractWithPlayerStash() const
{
	if(PlayerStashComponent)
	{
		return PlayerStashComponent->CanOwnerModifyPlayerStashState();
	}
	return false;
}

bool UObsidianInventoryItemsWidgetController::GetDraggedItemGridSpan(FIntPoint& OutItemGridSpan) const
{
	if(!IsDraggingAnItem())
	{
		return false;
	}
	
	const FDraggedItem DraggedItem = OwnerPlayerInputManager->GetDraggedItem();
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

UObsidianItem* UObsidianInventoryItemsWidgetController::GetItemWidgetAtInventoryGridSlot(const FIntPoint& AtGridSlot) const
{
	if(AddedItemWidgetMap.Contains(AtGridSlot))
	{
		return AddedItemWidgetMap[AtGridSlot];
	}
	return nullptr;
}

void UObsidianInventoryItemsWidgetController::RegisterInventoryItemWidget(const FIntPoint& GridSlot, UObsidianItem* ItemWidget)
{
	if(!AddedItemWidgetMap.Contains(GridSlot))
	{
		AddedItemWidgetMap.Add(GridSlot, ItemWidget);
	}
}

void UObsidianInventoryItemsWidgetController::RemoveInventoryItemWidget(const FIntPoint& GridSlot)
{
	if(AddedItemWidgetMap.Contains(GridSlot))
	{
		if(UObsidianItem* Item = AddedItemWidgetMap[GridSlot])
		{
			if(ActiveItemDescription && ActiveItemDescription->IsInventoryItemDescription() && ActiveItemDescription->GetAssociatedItemPosition() == GridSlot)
			{
				RemoveCurrentItemDescription();
			}
			Item->RemoveFromParent();
		}
		AddedItemWidgetMap.Remove(GridSlot);
	}
}

UObsidianItem* UObsidianInventoryItemsWidgetController::GetItemWidgetAtEquipmentSlot(const FGameplayTag& Slot) const
{
	if(EquippedItemWidgetMap.Contains(Slot))
	{
		return EquippedItemWidgetMap[Slot];
	}
	return nullptr;
}

void UObsidianInventoryItemsWidgetController::RegisterEquipmentItemWidget(const FGameplayTag& Slot, UObsidianItem* ItemWidget, const bool bSwappedWithAnother)
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

void UObsidianInventoryItemsWidgetController::RegisterCurrentStashTab(const FGameplayTag& CurrentStashTab)
{
	if (PlayerStashComponent)
	{
		PlayerStashComponent->ServerRegisterAndValidateCurrentStashTab(CurrentStashTab);
	}
}

UObsidianItem* UObsidianInventoryItemsWidgetController::GetItemWidgetAtStashPosition(const FObsidianItemPosition& ItemPosition) const
{
	const FGameplayTag StashTabTag = ItemPosition.GetOwningStashTabTag();
	
	const FObsidianStashAddedItemWidgets* Stash = StashAddedItemWidgets.FindByPredicate(
		[&](const FObsidianStashAddedItemWidgets& PotentialStash)
		{
			return PotentialStash.GetStashTag() == StashTabTag;
		});

	if (Stash)
	{
		return Stash->GetItemWidgetAtPosition(ItemPosition);
	}
	
	return nullptr;
}

void UObsidianInventoryItemsWidgetController::RegisterStashTabItemWidget(const FObsidianItemPosition& ItemPosition, UObsidianItem* ItemWidget)
{
	const FGameplayTag StashTabTag = ItemPosition.GetOwningStashTabTag();
	
	FObsidianStashAddedItemWidgets* Stash = StashAddedItemWidgets.FindByPredicate(
		[&](const FObsidianStashAddedItemWidgets& PotentialStash)
		{
			return PotentialStash.GetStashTag() == StashTabTag;
		});

	if(Stash == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Warning, TEXT("[%s] Gameplay Tag wasn't found inside StashAddedItemWidgets, added new one."), *StashTabTag.GetTagName().ToString());
		Stash = &StashAddedItemWidgets.Emplace_GetRef(StashTabTag);
	}
	
	Stash->AddItemWidget(ItemPosition, ItemWidget);
	Stash->DebugPrintAllAddedItems();
}

void UObsidianInventoryItemsWidgetController::RemoveStashItemWidget(const FObsidianItemPosition& ItemPosition)
{
	FObsidianStashAddedItemWidgets* Stash = StashAddedItemWidgets.FindByPredicate(
		[&](const FObsidianStashAddedItemWidgets& PotentialStash)
		{
			return PotentialStash.GetStashTag() == ItemPosition.GetOwningStashTabTag();
		});

	if (ensure(Stash))
	{
		if (Stash->RemoveItemWidget(ItemPosition))
		{
			// if(ActiveItemDescription && ActiveItemDescription->IsEquipmentDescription() && ActiveItemDescription->GetAssociatedItemPosition() == Slot)
			// {
			// 	RemoveCurrentItemDescription();
			// }
			
			Stash->DebugPrintAllAddedItems();
		}
	}
}

void UObsidianInventoryItemsWidgetController::AddBlockedEquipmentItemWidget(const FGameplayTag& PrimarySlot, UObsidianSlotBlockadeItem* ItemWidget, const bool bSwappedWithAnother)
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

void UObsidianInventoryItemsWidgetController::RemoveEquipmentItemWidget(const FGameplayTag& Slot)
{
	if(EquippedItemWidgetMap.Contains(Slot))
	{
		if(UObsidianItem* Item = EquippedItemWidgetMap[Slot])
		{
			if(ActiveItemDescription && ActiveItemDescription->IsEquipmentDescription() && ActiveItemDescription->GetAssociatedItemPosition() == Slot)
			{
				RemoveCurrentItemDescription();
			}
			Item->RemoveFromParent();
		}
		EquippedItemWidgetMap.Remove(Slot);
	}
}

void UObsidianInventoryItemsWidgetController::RemoveBlockedSlotItemWidget(const FGameplayTag& Slot)
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

bool UObsidianInventoryItemsWidgetController::CanEquipDraggedItem(const FGameplayTag& SlotTag) const
{
	if(OwnerPlayerInputManager == nullptr || EquipmentComponent == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("OwnerPlayerInputManager or EquipmentComponent is invalid in UObsidianInventoryItemsWidgetController::CanEquipDraggedItem."))
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

bool UObsidianInventoryItemsWidgetController::CanInteractWithEquipment() const
{
	if(EquipmentComponent)
	{
		return EquipmentComponent->CanOwnerModifyEquipmentState();
	}
	return false;
}

UObsidianItemDescriptionBase* UObsidianInventoryItemsWidgetController::CreateInventoryItemDescription(const UObsidianItem* ForItemWidget, const FObsidianItemStats& ItemStats)
{
	RemoveCurrentItemDescription(); // Clear any other Item Description
	
	checkf(ItemDescriptionClass, TEXT("Tried to create widget without valid widget class, fill it in UObsidianInventoryItemsWidgetController instance."));
	ActiveItemDescription = CreateWidget<UObsidianItemDescriptionBase>(ObsidianPlayerController, ItemDescriptionClass);
	ActiveItemDescription->InitializeWidgetWithItemStats(ItemStats);
	ActiveItemDescription->AddToViewport();
	
	const FVector2D DescriptionViewportPosition = CalculateDescriptionPosition(ForItemWidget);
	ActiveItemDescription->SetPositionInViewport(DescriptionViewportPosition);
	bDescriptionActive = true;

	return ActiveItemDescription;
}

UObsidianItemDescriptionBase* UObsidianInventoryItemsWidgetController::CreateDroppedItemDescription(const FObsidianItemStats& ItemStats)
{
	check(ObsidianPlayerController);
	
	AObsidianHUD* ObsidianHUD = ObsidianPlayerController->GetObsidianHUD();
	if(ObsidianHUD == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Unable to get ObsidianHUD in UObsidianInventoryItemsWidgetController::CreateItemDescriptionForDroppedItem."));
		return nullptr;
	}

	const UObsidianMainOverlay* MainOverlay = ObsidianHUD->GetMainOverlay();
	if(MainOverlay == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Unable to get ObsidianMainOverlay in UObsidianInventoryItemsWidgetController::CreateItemDescriptionForDroppedItem."));
		return nullptr;
	}

	RemoveCurrentItemDescription(); // Clear any other Item Description

	checkf(ItemDescriptionClass, TEXT("Tried to create widget without valid widget class, fill it in UObsidianInventoryItemsWidgetController instance."));
	ActiveItemDescription = CreateWidget<UObsidianItemDescriptionBase>(ObsidianPlayerController, ItemDescriptionClass);
	ActiveItemDescription->InitializeWidgetWithItemStats(ItemStats, true);
	MainOverlay->AddItemDescriptionToOverlay(ActiveItemDescription);
	bDescriptionActive = true;
	
	return ActiveItemDescription;
}

void UObsidianInventoryItemsWidgetController::RegisterInitialStashTabs()
{
	UObsidianStashTabsConfig* StashConfig = PlayerStashComponent->GetStashTabConfig();
	for(const FObsidianStashTabDefinition& Definition : StashConfig->GetStashTabDefinitions())
	{
		StashAddedItemWidgets.Add(FObsidianStashAddedItemWidgets(Definition.StashTag));
	}
}

void UObsidianInventoryItemsWidgetController::EmptyRegisteredItems()
{
	for(FObsidianStashAddedItemWidgets& Stash : StashAddedItemWidgets)
	{
		const int32 NumberOfAddedWidgets = Stash.GetNumberOfItemsAdded();
		Stash.EmptyAddedItemWidgets(NumberOfAddedWidgets);
	}
}

FVector2D UObsidianInventoryItemsWidgetController::CalculateUnstackSliderPosition(const UObsidianItem* ItemWidget) const
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

FVector2D UObsidianInventoryItemsWidgetController::CalculateDescriptionPosition(const UObsidianItem* ItemWidget) const
{
	FVector2D FinalPosition = FVector2D::Zero();
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Failed to calculate Description Position"));
		return FinalPosition;
	}

	if(ItemWidget == nullptr || ActiveItemDescription == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Failed to calculate Description Position"));
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

FVector2D UObsidianInventoryItemsWidgetController::GetItemUIElementPositionBoundByViewport(const FVector2D& ViewportSize, const FVector2D& ItemPosition, const FVector2D& ItemSize, const FVector2D& UIElementSize) const
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



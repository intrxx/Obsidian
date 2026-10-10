// Copyright 2026 out of sCope team - intrxx

#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/Crafting/ObsidianCraftingComponent.h"
#include "InventoryItems/Equipment/ObsidianEquipmentList.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemManagerComponent.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "InventoryItems/PlayerStash/ObsidianStashTab.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "Obsidian/ObsidianLogCategories.h"
#include "UI/InventoryItems/Items/ObsidianDraggedItem.h"
#include "UI/InventoryItems/Items/ObsidianItem.h"
#include "UI/InventoryItems/Items/ObsidianItemDescriptionBase.h"
#include "UI/InventoryItems/Items/ObsidianUnstackSlider.h"
#include "UI/MainOverlay/ObsidianMainOverlay.h"
#include "UI/ObsidianHUD.h"


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
	check(OwnerPlayerController.IsValid());
	const AObsidianPlayerController* PlayerController = OwnerPlayerController.Get();
	if (PlayerController == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("PlayerController is invalid in [%hs]."), __FUNCTION__);
	}
	
	OwnerCraftingComponent = PlayerController->GetCraftingComponent();
	check(OwnerCraftingComponent.IsValid())
	OwnerInventoryComponent = PlayerController->GetInventoryComponent();
	check(OwnerInventoryComponent.IsValid())
	OwnerEquipmentComponent = PlayerController->GetEquipmentComponent();
	check(OwnerEquipmentComponent.IsValid())
	OwnerPlayerStashComponent = PlayerController->GetPlayerStashComponent();
	check(OwnerPlayerStashComponent.IsValid())

	const AActor* OwningActor = Cast<AActor>(OwnerPlayerController->GetPawn());
	check(OwningActor);
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwningActor->GetWorld());
	MessageSubsystem.RegisterListener(ObsidianGameplayTags::Message::Inventory::Changed, this,
		&ThisClass::OnInventoryStateChanged);
	MessageSubsystem.RegisterListener(ObsidianGameplayTags::Message::Equipment::Changed, this,
		&ThisClass::OnEquipmentStateChanged);
	MessageSubsystem.RegisterListener(ObsidianGameplayTags::Message::PlayerStash::Changed, this,
		&ThisClass::OnPlayerStashChanged);
	
	OwnerItemManagerComponent = OwnerPlayerController->GetItemManagerComponent();
	check(OwnerItemManagerComponent.IsValid());
	if (UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get())
	{
		ItemManager->OnStartDraggingItemDelegate.AddUObject(this, &ThisClass::OnStartDraggingItem);
		ItemManager->OnStopDraggingItemDelegate.AddUObject(this, &ThisClass::OnStopDraggingItem);
	}
}

void UObInventoryItemsWidgetController::OnInventoryStateChanged(FGameplayTag InChannel,
	const FObsidianInventoryChangeMessage& InInventoryChangeMessage)
{
	// Fixes a bug when Items appear in Server's Inventory (Listen Server Character) after picked up by client.
	if(OwnerInventoryComponent != InInventoryChangeMessage.InventoryOwner)
	{
		return;
	}
	
	const UObsidianInventoryItemInstance* Instance = InInventoryChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Item Instance is invalid in [%hs]"), __FUNCTION__);
		return;
	}

	if(InInventoryChangeMessage.ChangeType == EObsidianInventoryChangeType::ICT_ItemAdded)
	{
		UE_LOG(ObLogUIItems, Verbose, TEXT("[Widget] Adding item: [%s] to Inventory"),
			*Instance->GetItemDisplayName().ToString());
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemImage = Instance->GetItemImage();
		ItemWidgetData.ItemPosition = InInventoryChangeMessage.GridItemPosition;
		ItemWidgetData.GridSpan = Instance->GetItemGridSpan();
		ItemWidgetData.StackCount = Instance->IsStackable() ? InInventoryChangeMessage.NewCount : 0;
		ItemWidgetData.bUsable = Instance->IsItemUsable();
		ItemWidgetData.ItemSlotPadding = Instance->GetItemSlotPadding();
		
		OnItemInventorizedDelegate.Broadcast(ItemWidgetData);
	}
	else if(InInventoryChangeMessage.ChangeType == EObsidianInventoryChangeType::ICT_ItemRemoved)
	{
		UE_LOG(ObLogUIItems, Verbose, TEXT("[Widget] Removing item: [%s] from Inventory"),
			*Instance->GetItemDisplayName().ToString());
		
		ClearItemDescriptionForPosition(InInventoryChangeMessage.GridItemPosition);

		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemPosition = InInventoryChangeMessage.GridItemPosition;
		OnInventorizedItemRemovedDelegate.Broadcast(ItemWidgetData);
	}
	else if (InInventoryChangeMessage.ChangeType == EObsidianInventoryChangeType::ICT_ItemStacksChanged)
	{
		UE_LOG(ObLogUIItems, Verbose, TEXT("[Widget] Changing item: [%s] in Inventory"),
			*Instance->GetItemDisplayName().ToString());
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemPosition = InInventoryChangeMessage.GridItemPosition;
		ItemWidgetData.StackCount = Instance->IsStackable() ? InInventoryChangeMessage.NewCount : 0;
		ItemWidgetData.bUpdateStacks = true;
		
		OnInventoryItemChangedDelegate.Broadcast(ItemWidgetData);
	}
	else if(InInventoryChangeMessage.ChangeType == EObsidianInventoryChangeType::ICT_GeneralItemChanged)
	{
		//TODO(intrxx) Fix highlight
		// HandleHoveringOverItem(InventoryChangeMessage.GridItemPosition, nullptr);
		// if(UObsidianItem* CorrespondingItemWidget = GetItemWidgetFromInventoryAtGridPosition(InventoryChangeMessage.GridItemPosition))
		// {
		// 	CorrespondingItemWidget->ResetHighlight();
		// 	CachedItemsMatchingUsableContext.Remove(CorrespondingItemWidget);
		// }
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemPosition = InInventoryChangeMessage.GridItemPosition;
		ItemWidgetData.bGeneralItemUpdate = true;
		
		OnInventoryItemChangedDelegate.Broadcast(ItemWidgetData);
	}
}

void UObInventoryItemsWidgetController::OnEquipmentStateChanged(FGameplayTag InChannel,
	const FObsidianEquipmentChangeMessage& InEquipmentChangeMessage)
{
	// Fixes a bug when Items appear in Server's Inventory (Listen Server Character) after picked up by client.
	if(OwnerEquipmentComponent != InEquipmentChangeMessage.EquipmentOwner) 
	{
		return;
	}
	
	const UObsidianInventoryItemInstance* Instance = InEquipmentChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Inventory Item Instance is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}

	if(InEquipmentChangeMessage.ChangeType == EObsidianEquipmentChangeType::ECT_ItemEquipped)
	{
		UE_LOG(ObLogUIItems, Verbose, TEXT("[Widget] Equipping item: [%s]"),
			*Instance->GetItemDisplayName().ToString());

		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemImage = Instance->GetItemImage();
		ItemWidgetData.ItemPosition = InEquipmentChangeMessage.SlotTag;
		ItemWidgetData.GridSpan = Instance->GetItemGridSpan();
		ItemWidgetData.bDoesBlockSisterSlot = Instance->DoesItemNeedTwoSlots();
		ItemWidgetData.ItemCategory = Instance->GetItemCategoryTag();
		ItemWidgetData.ItemSlotPadding = Instance->GetItemSlotPadding();
		
		OnItemEquippedDelegate.Broadcast(ItemWidgetData);
	}
	else if(InEquipmentChangeMessage.ChangeType == EObsidianEquipmentChangeType::ECT_ItemUnequipped)
	{
		UE_LOG(ObLogUIItems, Verbose, TEXT("[Widget] Unequipping item: [%s]"),
			*Instance->GetItemDisplayName().ToString());
		
		const FGameplayTag SlotTagToClear = InEquipmentChangeMessage.SlotTagToClear;
		if(SlotTagToClear.IsValid())
		{
			ClearItemDescriptionForPosition(SlotTagToClear);

			FObsidianItemWidgetData ItemWidgetData;
			ItemWidgetData.ItemPosition = SlotTagToClear;
			ItemWidgetData.bDoesBlockSisterSlot = Instance->DoesItemNeedTwoSlots();
			OnEquippedItemRemovedDelegate.Broadcast(ItemWidgetData);
		}
	}
	else if(InEquipmentChangeMessage.ChangeType == EObsidianEquipmentChangeType::ECT_ItemSwapped)
	{
		UE_LOG(ObLogUIItems, Verbose, TEXT("[Widget] Equipment Swapping item: [%s]"),
			*Instance->GetItemDisplayName().ToString());

		const FGameplayTag SlotTagToClear = InEquipmentChangeMessage.SlotTagToClear;
		if(SlotTagToClear.IsValid())
		{
			FObsidianItemWidgetData ItemWidgetData;
			ItemWidgetData.ItemPosition = SlotTagToClear;
			ItemWidgetData.bDoesBlockSisterSlot = Instance->DoesItemNeedTwoSlots();
			OnEquippedItemRemovedDelegate.Broadcast(ItemWidgetData);
		}
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemImage = Instance->GetItemImage();
		ItemWidgetData.ItemPosition = InEquipmentChangeMessage.SlotTag;
		ItemWidgetData.GridSpan = Instance->GetItemGridSpan();
		ItemWidgetData.bSwappedWithAnotherItem = InEquipmentChangeMessage.SlotTagToClear == FGameplayTag::EmptyTag;
		ItemWidgetData.bDoesBlockSisterSlot = Instance->DoesItemNeedTwoSlots();
		ItemWidgetData.ItemCategory = Instance->GetItemCategoryTag();
		ItemWidgetData.ItemSlotPadding = Instance->GetItemSlotPadding();
		
		OnItemEquippedDelegate.Broadcast(ItemWidgetData);
	}
}

void UObInventoryItemsWidgetController::OnPlayerStashChanged(FGameplayTag InChannel,
	const FObsidianStashChangeMessage& InStashChangeMessage)
{
	// Fixes a bug when Items appear in Server's Inventory (Listen Server Character) after picked up by client.
	if(OwnerPlayerStashComponent != InStashChangeMessage.PlayerStashOwner)
	{
		return;
	}
	
	const UObsidianInventoryItemInstance* Instance = InStashChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Item Instance is invalid in [%hs]"), __FUNCTION__);
		return;
	}

	if(InStashChangeMessage.ChangeType == EObsidianStashChangeType::ICT_ItemAdded)
	{
		UE_LOG(ObLogUIItems, Verbose, TEXT("[Widget] Adding item: [%s] to Player Stash"),
			*Instance->GetItemDisplayName().ToString());
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemImage = Instance->GetItemImage();
		ItemWidgetData.ItemPosition = InStashChangeMessage.ItemPosition;
		ItemWidgetData.GridSpan = Instance->GetItemGridSpan();
		ItemWidgetData.StackCount = Instance->IsStackable() ? InStashChangeMessage.NewCount : 0;
		ItemWidgetData.bUsable = Instance->IsItemUsable();
		ItemWidgetData.ItemSlotPadding = Instance->GetItemSlotPadding();
		
		OnItemStashedDelegate.Broadcast(ItemWidgetData);
	}
	else if(InStashChangeMessage.ChangeType == EObsidianStashChangeType::ICT_ItemRemoved)
	{
		UE_LOG(ObLogUIItems, Verbose, TEXT("[Widget] Removing item: [%s] from Player Stash"),
			*Instance->GetItemDisplayName().ToString());

		ClearItemDescriptionForPosition(InStashChangeMessage.ItemPosition);
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemPosition = InStashChangeMessage.ItemPosition;
		OnStashedItemRemovedDelegate.Broadcast(ItemWidgetData);
	}
	else if (InStashChangeMessage.ChangeType == EObsidianStashChangeType::ICT_ItemStacksChanged)
	{
		UE_LOG(ObLogUIItems, Verbose, TEXT("[Widget] Changing item: [%s] in Player Stash"),
			*Instance->GetItemDisplayName().ToString());
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemPosition = InStashChangeMessage.ItemPosition;
		ItemWidgetData.StackCount = Instance->IsStackable() ? InStashChangeMessage.NewCount : 0;
		ItemWidgetData.bUpdateStacks = true;
		
		OnStashedItemChangedDelegate.Broadcast(ItemWidgetData);
	}
	else if(InStashChangeMessage.ChangeType == EObsidianStashChangeType::ICT_GeneralItemChanged)
	{
		// HandleHoveringOverItem(StashChangeMessage.GridItemPosition);
		// if(UObsidianItem* CorrespondingItemWidget = GetItemWidgetFromInventoryAtGridPosition(InventoryChangeMessage.GridItemPosition))
		// {
		// 	CorrespondingItemWidget->ResetHighlight();
		// 	CachedItemsMatchingUsableContext.Remove(CorrespondingItemWidget);
		// }
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemPosition = InStashChangeMessage.ItemPosition;
		ItemWidgetData.bGeneralItemUpdate = true;
		
		OnStashedItemChangedDelegate.Broadcast(ItemWidgetData);
	}
}

void UObInventoryItemsWidgetController::OnInventoryOpen()
{
	TArray<UObsidianInventoryItemInstance*> InventoryItems = OwnerInventoryComponent->GetAllItems();
	for(const UObsidianInventoryItemInstance* Item : InventoryItems)
	{
		if(ensure(Item))
		{
			FObsidianItemWidgetData ItemWidgetData;
			ItemWidgetData.ItemImage = Item->GetItemImage();
			ItemWidgetData.GridSpan = Item->GetItemGridSpan();
			ItemWidgetData.ItemPosition = Item->GetItemCurrentPosition();
			ItemWidgetData.StackCount = Item->IsStackable() ?
										Item->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current) :
										0;
			ItemWidgetData.bUsable = Item->IsItemUsable();
			ItemWidgetData.ItemSlotPadding = Item->GetItemSlotPadding();

			OnItemInventorizedDelegate.Broadcast(ItemWidgetData);
		}
	}

	TArray<UObsidianInventoryItemInstance*> EquippedItems = OwnerEquipmentComponent->GetAllEquippedItems();
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
	//TODO(intrxx) This for sure will need to be changed, it will be to heavy on performance.
	TArray<UObsidianInventoryItemInstance*> StashedItems = OwnerPlayerStashComponent->GetAllItems();
	for(const UObsidianInventoryItemInstance* Instance : StashedItems)
	{
		if(ensure(Instance))
		{
			FObsidianItemWidgetData ItemWidgetData;
			ItemWidgetData.ItemImage = Instance->GetItemImage();
			ItemWidgetData.ItemPosition = Instance->GetItemCurrentPosition();
			ItemWidgetData.GridSpan = Instance->GetItemGridSpan();
			ItemWidgetData.StackCount = Instance->IsStackable() ?
										Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current) :
										0;
			ItemWidgetData.bUsable = Instance->IsItemUsable();
			ItemWidgetData.ItemSlotPadding = Instance->GetItemSlotPadding();
		
			OnItemStashedDelegate.Broadcast(ItemWidgetData);
		}
	}
}

UObsidianItemDescriptionBase* UObInventoryItemsWidgetController::GetActiveDroppedItemDescription()
{
	return ActiveDroppedItemDescription;
}

TConstArrayView<TObjectPtr<UObsidianStashTab>> UObInventoryItemsWidgetController::GetAllStashTabs() const
{
	if (const UObsidianPlayerStashComponent* StashComp = OwnerPlayerStashComponent.Get())
	{
		return StashComp->GetAllStashTabs();
	}
	
	UE_LOG(ObLogUIItems, Error, TEXT("Trying to GetAllStashTabs but Stash Component is invalid in"
											   " [%hs]"), __FUNCTION__);
	return {};	
}

FString UObInventoryItemsWidgetController::GetStashTabName(const FGameplayTag InStashTabTag) const
{
	if (UObsidianPlayerStashComponent* PlayerStashComp = OwnerPlayerStashComponent.Get())
	{
		if (const UObsidianStashTab* StashTab = PlayerStashComp->GetStashTabForTag(InStashTabTag))
		{
			return StashTab->GetStashTabName();
		}
	}
	return FString();
}

int32 UObInventoryItemsWidgetController::GetInventoryGridWidth() const
{
	if (const UObsidianInventoryComponent* InventoryComp = OwnerInventoryComponent.Get())
	{
		return InventoryComp->GetInventoryGridWidth();
	}
	
	UE_LOG(ObLogUIItems, Error, TEXT("Trying to return Grid Width but Inventory Component is"
											   " invalid in [%hs]"), __FUNCTION__);
	return 0;
}

int32 UObInventoryItemsWidgetController::GetInventoryGridHeight() const
{
	if (const UObsidianInventoryComponent* InventoryComp = OwnerInventoryComponent.Get())
	{
		return InventoryComp->GetInventoryGridHeight();
	}
	
	UE_LOG(ObLogUIItems, Error, TEXT("Trying to return Grid Height but Inventory Component is"
											   " invalid in [%hs]"), __FUNCTION__);
	return 0;
}

bool UObInventoryItemsWidgetController::IsDraggingAnItem() const
{
	check(OwnerItemManagerComponent.IsValid());
	if(const UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get())
	{
		return ItemManager->IsDraggingAnItem();
	}
	return false;
}

FIntPoint UObInventoryItemsWidgetController::GetDraggedItemGridSpan() const
{
	if(!IsDraggingAnItem())
	{
		return FIntPoint::NoneValue;
	}

	check(OwnerItemManagerComponent.IsValid());
	UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get();
	if (ItemManager == nullptr)
	{
		return false;
	}
	
	const FDraggedItem DraggedItem = ItemManager->GetDraggedItem();
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

FIntPoint UObInventoryItemsWidgetController::GetItemGridSpanByPosition(const FObsidianItemPosition& InItemPosition) const
{
	if (InItemPosition.IsValid() == false)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Trying to retrieve Item Grid Span from invalid Position."));
		return FIntPoint::NoneValue; 
	}

	if (InItemPosition.IsOnInventoryGrid())
	{
		if (const UObsidianInventoryItemInstance* Instance = OwnerInventoryComponent->GetItemInstanceAtLocation(
			InItemPosition.GetItemGridPosition()))
		{
			return Instance->GetItemGridSpan();
		}
	}
	else if (InItemPosition.IsOnStash())
	{
		if (const UObsidianInventoryItemInstance* Instance = OwnerPlayerStashComponent->GetItemInstanceFromTabAtPosition(
			InItemPosition))
		{
			return Instance->GetItemGridSpan();
		}
	}
	else if (InItemPosition.IsOnEquipmentSlot())
	{
		if (const UObsidianInventoryItemInstance* Instance = OwnerEquipmentComponent->GetEquippedInstanceAtSlot(
			InItemPosition.GetItemSlotTag()))
		{
			return Instance->GetItemGridSpan();
		}
	}
	
	UE_LOG(ObLogUIItems, Error, TEXT("Was unable to retrieve Item Grid Span from invalid Position."));
	return FIntPoint::NoneValue;
}

bool UObInventoryItemsWidgetController::CanInteractWithGrid(const EObsidianPanelOwner InPanelOwner) const
{
	switch (InPanelOwner)
	{
	case EObsidianPanelOwner::Inventory:
		check(OwnerInventoryComponent.IsValid());
		if (UObsidianInventoryComponent* InventoryComp = OwnerInventoryComponent.Get())
		{
			return InventoryComp->CanOwnerModifyInventoryState();
		}
		break;
	case EObsidianPanelOwner::PlayerStash:
		check(OwnerPlayerStashComponent.IsValid());
		if (UObsidianPlayerStashComponent* PlayerStashComp = OwnerPlayerStashComponent.Get())
		{
			return PlayerStashComp->CanOwnerModifyPlayerStashState();
		}
		break;
	default:
		UE_LOG(ObLogUIItems, Error, TEXT("There is no valid PanelOwner in [%hs]"), __FUNCTION__);
		break;
	}
	return false;
}

bool UObInventoryItemsWidgetController::CanInteractWithSlots(const EObsidianPanelOwner InPanelOwner) const
{
	switch (InPanelOwner)
	{
	case EObsidianPanelOwner::Equipment:
		check(OwnerEquipmentComponent.IsValid());
		if (UObsidianEquipmentComponent* EquipmentComp = OwnerEquipmentComponent.Get())
		{
			return EquipmentComp->CanOwnerModifyEquipmentState();
		}
		break;
	case EObsidianPanelOwner::PlayerStash:
		check(OwnerPlayerStashComponent.IsValid());
		if (UObsidianPlayerStashComponent* PlayerStashComp = OwnerPlayerStashComponent.Get())
		{
			return PlayerStashComp->CanOwnerModifyPlayerStashState();
		}
		break;
	default:
		UE_LOG(ObLogUIItems, Error, TEXT("There is no valid PanelOwner in [%hs]"), __FUNCTION__);
		break;
	}
	return false;
}

bool UObInventoryItemsWidgetController::CanInteractWithInventory() const
{
	if (UObsidianInventoryComponent* InventoryComp = OwnerInventoryComponent.Get())
	{
		return InventoryComp->CanOwnerModifyInventoryState();
	}
	return false;
}

bool UObInventoryItemsWidgetController::CanInteractWithPlayerStash() const
{
	if (UObsidianPlayerStashComponent* PlayerStashComp = OwnerPlayerStashComponent.Get())
	{
		return PlayerStashComp->CanOwnerModifyPlayerStashState();
	}
	return false;
}

bool UObInventoryItemsWidgetController::CanInteractWithEquipment() const
{
	if (UObsidianEquipmentComponent* EquipmentComp = OwnerEquipmentComponent.Get())
	{
		return EquipmentComp->CanOwnerModifyEquipmentState();
	}
	return false;
}

bool UObInventoryItemsWidgetController::CanPlaceDraggedItemAtPosition(const FObsidianItemPosition& InAtPosition,
	const EObsidianPanelOwner InPanelOwner) const
{
	if (IsDraggingAnItem() == false)
	{
		return false;
	}
	
	check((uint8)InPanelOwner > 0);
	switch (InPanelOwner)
	{
	case EObsidianPanelOwner::Inventory:
		{
			ensureMsgf(InAtPosition.IsOnInventoryGrid(), TEXT("Trying to add item to Inventory with"
													" invalid Inventory position."));
			
			return CanPlaceDraggedItemInInventory(InAtPosition.GetItemGridPosition());
		}
	case EObsidianPanelOwner::Equipment:
		{
			ensureMsgf(InAtPosition.IsOnEquipmentSlot(), TEXT("Trying to add item to Equipment with"
													" invalid Equipment position."));
				
			return CanPlaceDraggedItemInEquipment(InAtPosition.GetItemSlotTag());
		}
	case EObsidianPanelOwner::PlayerStash:
		{
			ensureMsgf(InAtPosition.IsOnStash(), TEXT("Trying to add item to Stash with"
													" invalid Stash position."));

			return CanPlaceDraggedItemInStash(InAtPosition);
		}
	default:
		{
			UE_LOG(ObLogUIItems, Error, TEXT("[%d] PanelOwner is invalid in [%hs]."),
				InPanelOwner, __FUNCTION__);
			return false;
		}
	}
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnSlot(const FObsidianItemPosition& InAtItemPosition,
	const FObsidianItemInteractionData& InInteractionData, const EObsidianPanelOwner InPanelOwner)
{
	check(OwnerCraftingComponent.IsValid());
	UObsidianCraftingComponent* CraftingComponent = OwnerCraftingComponent.Get();
	if (CraftingComponent->IsUsingItem())
	{
		CraftingComponent->SetUsingItem(false);
		return;
	}

	RequestAddingItem(InAtItemPosition, InInteractionData, InPanelOwner);
}

void UObInventoryItemsWidgetController::HandleRightClickingOnSlot(const FObsidianItemPosition& InAtItemPosition,
	const FObsidianItemInteractionData& InInteractionData, const EObsidianPanelOwner InPanelOwner)
{
	check(OwnerCraftingComponent.IsValid());
	UObsidianCraftingComponent* CraftingComponent = OwnerCraftingComponent.Get();
	if (CraftingComponent->IsUsingItem())
	{
		CraftingComponent->SetUsingItem(false);
	}
}

void UObInventoryItemsWidgetController::HandleRightClickingOnItem(const FObsidianItemPosition& InAtItemPosition,
                                                                  const FObsidianItemInteractionData& InInteractionData, const EObsidianPanelOwner InPanelOwner)
{
	check((uint8)InPanelOwner > 0);
	switch (InPanelOwner)
	{
		case EObsidianPanelOwner::Inventory:
			{
				ensureMsgf(InAtItemPosition.IsOnInventoryGrid(), TEXT("Trying to handle right click on Inventory"
														" item with invalid Inventory position."));
				
				HandleRightClickingOnInventoryItem(InAtItemPosition.GetItemGridPosition(), InInteractionData.ItemWidget);
				break;
			}
		case EObsidianPanelOwner::PlayerStash:
			{
				ensureMsgf(InAtItemPosition.IsOnStash(), TEXT("Trying to handle right click on Stash item with"
														" invalid Stash position."));
				HandleRightClickingOnStashedItem(InAtItemPosition, InInteractionData.ItemWidget);
				break;
			}
			default:
				{
					UE_LOG(ObLogUIItems, Error, TEXT("[%d] PanelOwner is invalid in [%hs]."),
						InPanelOwner, __FUNCTION__);
					break;
				}
	}
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnItem(const FObsidianItemPosition& InAtItemPosition,
	const FObsidianItemInteractionData& InInteractionData, const EObsidianPanelOwner InPanelOwner)
{
	check((uint8)InPanelOwner > 0);
	switch (InPanelOwner)
	{
		case EObsidianPanelOwner::Inventory:
			{
				ensureMsgf(InAtItemPosition.IsOnInventoryGrid(), TEXT("Trying to handle left click on Inventorized"
														" item with invalid Inventory position."));
				
				if(InInteractionData.InteractionFlags.bItemStacksInteraction)
				{
					HandleLeftClickingOnInventoryItemWithShiftDown(InAtItemPosition.GetItemGridPosition(),
						InInteractionData.ItemWidget);
				}
				else
				{
					ensureMsgf(InInteractionData.InteractionTargetPositionOverride.IsOnInventoryGrid(),
						TEXT("Trying to handle left click on Inventorized item with invalid Interaction"
						" Target Position Override."));
					
					HandleLeftClickingOnInventoryItem(InAtItemPosition.GetItemGridPosition(),
						InInteractionData.InteractionTargetPositionOverride.GetItemGridPosition(),
						InInteractionData.InteractionFlags.bMoveBetweenNextOpenedWindow);	
				}
				
				break;
			}
		case EObsidianPanelOwner::Equipment:
			{
				ensureMsgf(InAtItemPosition.IsOnEquipmentSlot(), TEXT("Trying to handle left click on Equipped"
														" item with invalid Equipment position."));

				if (InInteractionData.InteractionFlags.bInteractWithSisterSlottedItem)
				{
					HandleLeftClickingOnEquipmentItem(InAtItemPosition.GetItemSlotTag(),
						InInteractionData.InteractionTargetPositionOverride.GetItemSlotTag());
				}
				else
				{
					HandleLeftClickingOnEquipmentItem(InAtItemPosition.GetItemSlotTag());
				}
				
				break;
			}
		case EObsidianPanelOwner::PlayerStash:
			{
				ensureMsgf(InAtItemPosition.IsOnStash(), TEXT("Trying to handle left click on Stashed"
												" item with invalid Stash position."));

				if (InInteractionData.InteractionFlags.bItemStacksInteraction)
				{
					HandleLeftClickingOnStashedItemWithShiftDown(InAtItemPosition, InInteractionData.ItemWidget);
				}
				else
				{
					HandleLeftClickingOnStashedItem(InAtItemPosition,
						InInteractionData.InteractionFlags.bMoveBetweenNextOpenedWindow);
				}
				
				break;
			}
			default:
				{
					UE_LOG(ObLogUIItems, Error, TEXT("[%d] PanelOwner is invalid in [%hs]."),
						InPanelOwner, __FUNCTION__);
					break;
				}
	}
}

void UObInventoryItemsWidgetController::HandleHoveringOverItem(const FObsidianItemPosition& InItemPosition,
	const FObsidianItemInteractionData& InInteractionData, const EObsidianPanelOwner InPanelOwner)
{
	if (CanShowDescription() == false)
	{
		return;
	}
	
	if(InInteractionData.ItemWidget == nullptr || InItemPosition.IsValid() == false)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("ItemWidget or ItemPosition are invalid in [%hs]."),
			__FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* ForInstance = nullptr;
	switch (InPanelOwner)
	{
	case EObsidianPanelOwner::Inventory:
		{
			ensureMsgf(InItemPosition.IsOnInventoryGrid(), TEXT("Trying to hover over Inventorized Item with"
													" invalid Inventory position."));
				
			ForInstance = OwnerInventoryComponent->GetItemInstanceAtLocation(InItemPosition.GetItemGridPosition());
			break;
		}
	case EObsidianPanelOwner::Equipment:
		{
			ensureMsgf(InItemPosition.IsOnEquipmentSlot(), TEXT("Trying to hover over Equipped Item with"
													" invalid Equipment position."));
				
			ForInstance = OwnerEquipmentComponent->GetEquippedInstanceAtSlot(InItemPosition.GetItemSlotTag());
			break;
		}
	case EObsidianPanelOwner::PlayerStash:
		{
			ensureMsgf(InItemPosition.IsOnStash(), TEXT("Trying to hover over Stashed Item with"
													" invalid Stash position."));
				
			ForInstance = OwnerPlayerStashComponent->GetItemInstanceFromTabAtPosition(InItemPosition);
			break;
		}
	default:
		{
			break;
		}
	}

	if (ForInstance == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Was unable to retrieve the Item Instance from"
			" Item Position [%s] in [%hs]."), *InItemPosition.GetDebugStringPosition(), __FUNCTION__);
		return;
	}

	FObsidianItemStats OutItemStats;
	if (UObsidianItemsFunctionLibrary::GetItemStats(OwnerPlayerController.Get(), ForInstance, OutItemStats))
	{
		CreateInventoryItemDescription(InItemPosition, InPanelOwner, InInteractionData.ItemWidget, OutItemStats);
	}
}

void UObInventoryItemsWidgetController::HandleUnhoveringItem(const FObsidianItemPosition& InFromPosition)
{
	ClearItemDescriptionForPosition(InFromPosition);
}

void UObInventoryItemsWidgetController::RemoveItemUIElements(const EObsidianPanelOwner InForPanelOwner)
{
	ClearItemDescriptionsForOwner(InForPanelOwner);
	RemoveUnstackSlider();
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

void UObInventoryItemsWidgetController::CreateItemDescriptionForDroppedItem(const UObsidianInventoryItemInstance* InInstance)
{
	if(CanShowDescription() == false)
	{
		return;
	}
	
	FObsidianItemStats OutItemStats;
	if(UObsidianItemsFunctionLibrary::GetItemStats(OwnerPlayerController.Get(), InInstance, OutItemStats))
	{
		CreateDroppedItemDescription(OutItemStats);
	}
}

void UObInventoryItemsWidgetController::CreateItemDescriptionForDroppedItem(
	const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const FObsidianItemGeneratedData& InItemGeneratedData)
{
	if(!CanShowDescription())
	{
		return;
	}
	
	FObsidianItemStats OutItemStats;
	if(UObsidianItemsFunctionLibrary::GetItemStats_WithDef(OwnerPlayerController.Get(), InItemDef,
		InItemGeneratedData, OutItemStats))
	{
		CreateDroppedItemDescription(OutItemStats);
	}
}

void UObInventoryItemsWidgetController::RegisterCurrentStashTab(const FGameplayTag& InCurrentStashTab)
{
	if (UObsidianPlayerStashComponent* PlayerStashComp = OwnerPlayerStashComponent.Get())
	{
		PlayerStashComp->ServerRegisterAndValidateCurrentStashTab(InCurrentStashTab);
	}
	else
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Tried to register Current Stash Tab but the"
												" OwnerPlayerStashComponent is invalid in [%hs]"), __FUNCTION__);
	}
}

bool UObInventoryItemsWidgetController::CanPlaceDraggedItemInInventory(const FIntPoint& InAtGridSlot) const
{
	if(OwnerInventoryComponent == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("OwnerPlayerInputManager or OwnerInventoryComponent is"
			" invalid in [%hs]"), __FUNCTION__);
		return false;	
	}
	
	const FIntPoint DraggedItemGridSpan = GetDraggedItemGridSpan();
	if(DraggedItemGridSpan == FIntPoint::NoneValue)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Failed to retrieve Dragged Item Grid Span in [%hs]"),
			__FUNCTION__);
		return false;
	}
	
	return OwnerInventoryComponent->CheckSpecifiedPosition(DraggedItemGridSpan, InAtGridSlot);
}

bool UObInventoryItemsWidgetController::CanPlaceDraggedItemInStash(const FObsidianItemPosition& InItemPosition) const
{
	if (OwnerPlayerStashComponent == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("OwnerPlayerInputManager or OwnerPlayerStashComponent is"
			" invalid in [%hs]"), __FUNCTION__);
		return false;	
	}

	check(OwnerItemManagerComponent.IsValid());
	UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get();
	if (ItemManager == nullptr)
	{
		return false;
	}
	
	const FDraggedItem DraggedItem = ItemManager->GetDraggedItem();
	if (DraggedItem.IsEmpty())
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Failed to retrieve Dragged Item in  [%hs]"),
			__FUNCTION__);
		return false;
	}
	
	FGameplayTag CategoryTag;
	FGameplayTag ItemBaseType;
	const bool bSuccess = UObsidianItemsFunctionLibrary::GetItemCategoryAndBaseItemTypeTagsFromDraggedItem(DraggedItem,
		CategoryTag, ItemBaseType);
	if(bSuccess == false)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Failed to retrieve Dragged Item Category Tag and"
			" Item Base Type in [%hs]"), __FUNCTION__);
		return false;
	}

	const FIntPoint DraggedItemGridSpan = UObsidianItemsFunctionLibrary::GetGridSpanFromDraggedItem(DraggedItem);
	if(DraggedItemGridSpan == FIntPoint::NoneValue)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Failed to retrieve Dragged Item Grid Span in [%hs]"),
			__FUNCTION__);
		return false;
	}
	
	return OwnerPlayerStashComponent->CheckSpecifiedPosition(InItemPosition, CategoryTag, ItemBaseType, DraggedItemGridSpan);
}

bool UObInventoryItemsWidgetController::CanPlaceDraggedItemInEquipment(const FGameplayTag& InSlotTag) const
{
	check(OwnerItemManagerComponent.IsValid());
	UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get();
	if (ItemManager == nullptr)
	{
		UE_LOG(ObLogEquipment, Error, TEXT("InputManager is invalid in [%hs]."), __FUNCTION__);
		return false;
	}

	check(OwnerEquipmentComponent.IsValid());
	UObsidianEquipmentComponent* EquipmentComp = OwnerEquipmentComponent.Get();
	if (EquipmentComp == nullptr)
	{
		UE_LOG(ObLogEquipment, Error, TEXT("EquipmentComp is invalid in [%hs]."), __FUNCTION__);
		return false; 
	}

	const bool bSlotOccupied = EquipmentComp->IsItemEquippedAtSlot(InSlotTag);
	const FDraggedItem DraggedItem = ItemManager->GetDraggedItem();
	if(const UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance)
	{
		EObsidianEquipCheckResult EquipResult;
		if(bSlotOccupied)
		{
			EquipResult	= EquipmentComp->CanReplaceInstance(DraggedInstance, InSlotTag);
		}
		else
		{
			EquipResult	= EquipmentComp->CanEquipInstance(DraggedInstance, InSlotTag);
		}
		 
		return EquipResult == EObsidianEquipCheckResult::CanEquip;
	}
	if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef)
	{
		EObsidianEquipCheckResult EquipResult;
		if(bSlotOccupied)
		{
			EquipResult = EquipmentComp->CanReplaceTemplate(DraggedItemDef, InSlotTag, DraggedItem.GeneratedData);
		}
		else
		{
			EquipResult = EquipmentComp->CanEquipTemplate(DraggedItemDef, InSlotTag, DraggedItem.GeneratedData);
		}
		
		return EquipResult == EObsidianEquipCheckResult::CanEquip;
	}
	return false;
}

bool UObInventoryItemsWidgetController::CanShowDescription() const
{
	return !bUnstackSliderActive;
}

void UObInventoryItemsWidgetController::RequestAddingItem(const FObsidianItemPosition& InAtItemPosition,
	const FObsidianItemInteractionData& InInteractionData, const EObsidianPanelOwner InPanelOwner)
{
	if (IsDraggingAnItem() == false)
	{
		return;
	}
	
	check((uint8)InPanelOwner > 0);
	switch (InPanelOwner)
	{
	case EObsidianPanelOwner::Inventory:
		{
			ensureMsgf(InAtItemPosition.IsOnInventoryGrid(), TEXT("Trying to add item to Inventory with"
													" invalid Inventory position."));

			const bool bShiftDown = InInteractionData.InteractionFlags.bItemStacksInteraction;
			RequestAddingItemToInventory(InAtItemPosition.GetItemGridPosition(), bShiftDown);
			break;
		}
	case EObsidianPanelOwner::Equipment:
		{
			ensureMsgf(InAtItemPosition.IsOnEquipmentSlot(), TEXT("Trying to add item to Equipment with"
													" invalid Equipment position."));
				
			RequestAddingItemToEquipment(InAtItemPosition.GetItemSlotTag());
			break;
		}
	case EObsidianPanelOwner::PlayerStash:
		{
			ensureMsgf(InAtItemPosition.IsOnStash(), TEXT("Trying to add item to Stash with"
													" invalid Stash position."));

			const bool bShiftDown = InInteractionData.InteractionFlags.bItemStacksInteraction;
			RequestAddingItemToStashTab(InAtItemPosition, bShiftDown);
			break;
		}
	default:
		{
			UE_LOG(ObLogUIItems, Error, TEXT("[%d] PanelOwner is invalid in [%hs]."),
				InPanelOwner, __FUNCTION__);
			break;
		}
	}
}

void UObInventoryItemsWidgetController::RequestAddingItemToInventory(const FIntPoint& InToGridSlot, const bool bInShiftDown)
{
	check(OwnerItemManagerComponent.IsValid());
	UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get();
	if (ItemManager == nullptr)
	{
		return;
	}

	UObsidianCraftingComponent* CraftingComp = OwnerCraftingComponent.Get();
	if(CraftingComp && CraftingComp->IsUsingItem())
	{
		CraftingComp->SetUsingItem(false);
		return;
	}
	
	if(ItemManager->IsDraggingAnItem() == false)
	{
		return;
	}
	
	ItemManager->ServerAddItemToInventoryAtSlot(InToGridSlot, bInShiftDown);
}

void UObInventoryItemsWidgetController::RequestAddingItemToEquipment(const FGameplayTag& InSlotTag)
{
	check(OwnerItemManagerComponent.IsValid());
	UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get();
	if (ItemManager == nullptr)
	{
		return;
	}

	UObsidianCraftingComponent* CraftingComp = OwnerCraftingComponent.Get();
	if(CraftingComp && CraftingComp->IsUsingItem())
	{
		CraftingComp->SetUsingItem(false);
		return;
	}
	
	if(ItemManager->IsDraggingAnItem() == false)
	{
		return;
	}
	
	ItemManager->ServerEquipItemAtSlot(InSlotTag);
}

void UObInventoryItemsWidgetController::RequestAddingItemToStashTab(const FObsidianItemPosition& InToPosition,
	const bool bInShiftDown)
{
	check(OwnerItemManagerComponent.IsValid());
	UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get();
	if (ItemManager == nullptr)
	{
		return;
	}

	UObsidianCraftingComponent* CraftingComp = OwnerCraftingComponent.Get();
	if(CraftingComp && CraftingComp->IsUsingItem())
	{
		CraftingComp->SetUsingItem(false);
		return;
	}
	
	if(ItemManager->IsDraggingAnItem() == false)
	{
		return;
	}
	
	ItemManager->ServerAddItemToStashTabAtSlot(InToPosition, bInShiftDown);
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnInventoryItem(const FIntPoint& InClickedItemPosition,
	const FIntPoint& InClickedGridPosition, const bool bInAddToOtherWindow)
{
	check(DraggedItemWidgetClass);

	check(OwnerItemManagerComponent.IsValid());
	UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get();
	if (ItemManager == nullptr)
	{
		return;
	}

	check(OwnerInventoryComponent.IsValid());
	UObsidianInventoryComponent* InventoryComp = OwnerInventoryComponent.Get();
	if(InventoryComp == nullptr)
	{
		return;
	}

	if (InventoryComp->CanOwnerModifyInventoryState() == false)
	{
		return;
	}
	
	RemoveItemUIElements(EObsidianPanelOwner::Inventory);

	UObsidianCraftingComponent* CraftingComp = OwnerCraftingComponent.Get();
	if(CraftingComp && CraftingComp->IsUsingItem())
	{
		CraftingComp->UseItem(InClickedItemPosition, false);
		return;
	}

	if (ItemManager->IsDraggingAnItem() == false)
	{
		if (bInAddToOtherWindow == false)
		{
			ItemManager->ServerGrabInventoryItemToCursor(InClickedItemPosition);
			return;
		}

		AObsidianHUD* ObsidianHUD = OwnerPlayerController->GetObsidianHUD();
		if (ObsidianHUD && ObsidianHUD->IsPlayerStashOpened()) //TODO(intrxx) For now I support only Inventory <-> Stash
		{
			const FGameplayTag ToStashTab = ObsidianHUD->GetActiveStashTabTag(); //TODO(intrxx) This will need updating when I will support Stash Tab Affinities
			ItemManager->ServerTransferItemToPlayerStash(InClickedItemPosition, ToStashTab);
		}
		return;
	}
	
	const UObsidianInventoryItemInstance* InstanceToAddTo = InventoryComp->GetItemInstanceAtLocation(InClickedItemPosition);
	if (InstanceToAddTo == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Item Instance at pressed Location is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}
		
	const FDraggedItem DraggedItem = ItemManager->GetDraggedItem();
	if (UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance) // We carry item instance.
	{
		if (DraggedInstance->IsStackable() && UObsidianItemsFunctionLibrary::IsTheSameItem(DraggedInstance,
			InstanceToAddTo))
		{
			ItemManager->ServerAddStacksFromDraggedItemToInventoryItemAtSlot(InClickedItemPosition);
		}
		else if (InventoryComp->CanReplaceItemAtSpecificSlotWithInstance(InClickedItemPosition,
			InClickedGridPosition, DraggedInstance))
		{
			ItemManager->ServerReplaceItemAtInventorySlot(InClickedItemPosition, InClickedGridPosition);
		}
	}
	else if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef) // We carry item def
	{
		const UObsidianInventoryItemDefinition* DefaultObject = DraggedItemDef.GetDefaultObject();
		if (DefaultObject && DefaultObject->IsStackable() && UObsidianItemsFunctionLibrary::IsTheSameItem_WithDef(
																InstanceToAddTo, DraggedItemDef))
		{
			ItemManager->ServerAddStacksFromDraggedItemToInventoryItemAtSlot(InClickedItemPosition);
		}
		else if (InventoryComp->CanReplaceItemAtSpecificSlotWithDef(InClickedItemPosition,
			InClickedGridPosition, DraggedItemDef, DraggedItem.GeneratedData.GetStackCount()))
		{
			ItemManager->ServerReplaceItemAtInventorySlot(InClickedItemPosition, InClickedGridPosition);
		}
	}
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnInventoryItemWithShiftDown(const FIntPoint& InClickedItemPosition,
	const UObsidianItem* InItemWidget)
{
	check(OwnerItemManagerComponent.IsValid());
	UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get();
	if (ItemManager == nullptr)
	{
		return;
	}

	check(OwnerInventoryComponent.IsValid());
	UObsidianInventoryComponent* InventoryComp = OwnerInventoryComponent.Get();
	if(InventoryComp == nullptr)
	{
		return;
	}
	
	if(InventoryComp->CanOwnerModifyInventoryState() == false)
	{
		return;
	}
	
	UObsidianCraftingComponent* CraftingComp = OwnerCraftingComponent.Get();
	if(CraftingComp && CraftingComp->IsUsingItem())
	{
		CraftingComp->UseItem(InClickedItemPosition, true);
		return;
	}
	
	if(ItemManager->IsDraggingAnItem())
	{
		ItemManager->ServerAddStacksFromDraggedItemToInventoryItemAtSlot(InClickedItemPosition, 1);
		return;
	}
	
	UObsidianInventoryItemInstance* ItemInstance = InventoryComp->GetItemInstanceAtLocation(InClickedItemPosition);
	if(ItemInstance->IsStackable() == false)
	{
		return;
	}
	
	const int32 CurrentItemStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	if(CurrentItemStacks <= 1)
	{
		return;
	}
	
	RemoveItemUIElements(EObsidianPanelOwner::Inventory);

	checkf(UnstackSliderClass, TEXT("Tried to create widget without valid widget class in fill it in "
								 "UObInventoryItemsWidgetController instance."));
	ActiveUnstackSlider = CreateWidget<UObsidianUnstackSlider>(OwnerPlayerController.Get(), UnstackSliderClass);
	ActiveUnstackSlider->InitializeUnstackSlider(CurrentItemStacks, InClickedItemPosition);

	const FVector2D UnstackSliderViewportPosition = CalculateUnstackSliderPosition(InItemWidget);
	ActiveUnstackSlider->SetPositionInViewport(UnstackSliderViewportPosition);
	ActiveUnstackSlider->AddToViewport();
	bUnstackSliderActive = true;
	
	ActiveUnstackSlider->OnAcceptButtonPressedDelegate.AddUObject(this, &ThisClass::HandleTakingOutStacksFromInventory);
	ActiveUnstackSlider->OnCloseButtonPressedDelegate.AddUObject(this, &ThisClass::RemoveUnstackSlider);
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnEquipmentItem(const FGameplayTag& InSlotTag,
	const FGameplayTag& InEquipSlotTagOverride)
{
	const FGameplayTag SwapSlotTag = FGameplayTag::RequestGameplayTag(TEXT("Item.SwapSlot.Equipment"));
	if(InSlotTag.MatchesTag(SwapSlotTag))
	{
		//TODO(intrxx) Cannot left-click on swapped item, add VO?
		return;
	}
	
	check(OwnerItemManagerComponent.IsValid());
	UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get();
	if (ItemManager == nullptr)
	{
		return;
	}

	check(OwnerEquipmentComponent.IsValid());
	UObsidianEquipmentComponent* EquipmentComp = OwnerEquipmentComponent.Get();
	if(EquipmentComp->CanOwnerModifyEquipmentState() == false)
	{
		return;
	}
	
	RemoveItemUIElements(EObsidianPanelOwner::Equipment);

	UObsidianCraftingComponent* CraftingComp = OwnerCraftingComponent.Get();
	if(CraftingComp && CraftingComp->IsUsingItem())
	{
		CraftingComp->SetUsingItem(false);
		UE_LOG(ObLogUIItems, Error, TEXT("As of now it is impossible to use items onto equipped items,"
												" maybe will support it in the future."));
		return;
	}
	
	if(ItemManager->IsDraggingAnItem()) // If we carry an item, try to replace it with it.
	{
		
		 const FDraggedItem DraggedItem = ItemManager->GetDraggedItem();
		 if(UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance) // We carry item instance.
		 {
		 	const EObsidianEquipCheckResult EquipmentResult = EquipmentComp->CanReplaceInstance(DraggedInstance, InSlotTag);
		 	if(EquipmentResult == EObsidianEquipCheckResult::CanEquip)
		 	{
		 		ItemManager->ServerReplaceItemAtEquipmentSlot(InSlotTag, InEquipSlotTagOverride);
		 	}
		 	else
		 	{
		 		//TODO(intrxx) Send Client RPC with some voice over passing EquipResult?
#if !UE_BUILD_SHIPPING
		 		UE_LOG(ObLogEquipment, Verbose, TEXT("Item cannot be equipped, reason: [%s]"),
		 			*ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipmentResult));
#endif
		 	}
		 	return;
		 }
		
		 if(const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef) // We carry item def
		 {
		 	const EObsidianEquipCheckResult EquipmentResult = EquipmentComp->CanReplaceTemplate(DraggedItemDef, InSlotTag,
		 		DraggedItem.GeneratedData);
		 	if(EquipmentResult == EObsidianEquipCheckResult::CanEquip)
		 	{
		 		ItemManager->ServerReplaceItemAtEquipmentSlot(InSlotTag, InEquipSlotTagOverride);
		 	}
		 	else
		 	{
		 		//TODO(intrxx) Send Client RPC with some voice over passing EquipResult?
#if !UE_BUILD_SHIPPING
			 	UE_LOG(ObLogEquipment, Verbose, TEXT("Item cannot be equipped, reason: [%s]"),
			 		*ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipmentResult));
#endif
			 }
			 return;
		}
		return;
	}
	ItemManager->ServerGrabEquippedItemToCursor(InSlotTag);
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnStashedItem(const FObsidianItemPosition& InAtItemPosition,
	const bool bInAddToOtherWindow)
{
	check(OwnerPlayerStashComponent.IsValid());
	UObsidianPlayerStashComponent* PlayerStashComp = OwnerPlayerStashComponent.Get();
	if (PlayerStashComp == nullptr)
	{
		return;
	}

	if(PlayerStashComp->CanOwnerModifyPlayerStashState() == false)
	{
		return;
	}
	
	RemoveItemUIElements(EObsidianPanelOwner::PlayerStash);
	
	UObsidianCraftingComponent* CraftingComp = OwnerCraftingComponent.Get();
	if(CraftingComp && CraftingComp->IsUsingItem())
	{
		CraftingComp->UseItem(InAtItemPosition, false);
		return;
	}

	check(OwnerItemManagerComponent.IsValid());
	UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get();
	if (ItemManager == nullptr)
	{
		return;
	}

	if (ItemManager->IsDraggingAnItem() == false)
	{
		if (bInAddToOtherWindow == false)
		{
			ItemManager->ServerGrabStashedItemToCursor(InAtItemPosition);
			return;
		}

		const AObsidianHUD* ObsidianHUD = OwnerPlayerController->GetObsidianHUD();
		if (ObsidianHUD && ObsidianHUD->IsInventoryOpened()) //TODO(intrxx) For now I support only Inventory <-> Stash
		{
			ItemManager->ServerTransferItemToInventory(InAtItemPosition);
		}
		return;
	}
	
	
	const UObsidianInventoryItemInstance* InstanceToAddTo = PlayerStashComp->GetItemInstanceFromTabAtPosition(
		InAtItemPosition);
	if (InstanceToAddTo == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Item Instance at pressed Location is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}
		
	const FDraggedItem DraggedItem = ItemManager->GetDraggedItem();
	if (const UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance) // We carry item instance.
	{
		if (DraggedInstance->IsStackable() && UObsidianItemsFunctionLibrary::IsTheSameItem(
			DraggedInstance, InstanceToAddTo))
		{
			ItemManager->ServerAddStacksFromDraggedItemToStashedItemAtSlot(InAtItemPosition);
		}
		else if (PlayerStashComp->CanReplaceItemAtPosition(InAtItemPosition, DraggedInstance))
		{
			ItemManager->ServerReplaceItemAtStashPosition(InAtItemPosition);
		}
	}
	else if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef) // We carry item def
	{
		const UObsidianInventoryItemDefinition* DefaultObject = DraggedItemDef.GetDefaultObject();
		if (DefaultObject && DefaultObject->IsStackable() && UObsidianItemsFunctionLibrary::IsTheSameItem_WithDef(
			InstanceToAddTo, DraggedItemDef))
		{
			ItemManager->ServerAddStacksFromDraggedItemToStashedItemAtSlot(InAtItemPosition);
		}
		else if(PlayerStashComp->CanReplaceItemAtPosition(InAtItemPosition, DraggedItemDef))
		{
			ItemManager->ServerReplaceItemAtStashPosition(InAtItemPosition);
		}
	}
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnStashedItemWithShiftDown(
	const FObsidianItemPosition& InAtItemPosition, const UObsidianItem* InItemWidget)
{
	check(OwnerPlayerStashComponent.IsValid());
	UObsidianPlayerStashComponent* PlayerStashComp = OwnerPlayerStashComponent.Get();
	if (PlayerStashComp == nullptr)
	{
		return;
	}

	if(PlayerStashComp->CanOwnerModifyPlayerStashState() == false)
	{
		return;
	}
	
	UObsidianCraftingComponent* CraftingComp = OwnerCraftingComponent.Get();
	if(CraftingComp && CraftingComp->IsUsingItem())
	{
		CraftingComp->UseItem(InAtItemPosition, true);
		return;
	}

	check(OwnerItemManagerComponent.IsValid());
	UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get();
	if (ItemManager == nullptr)
	{
		return;
	}
	
	if(ItemManager->IsDraggingAnItem())
	{
		ItemManager->ServerAddStacksFromDraggedItemToStashedItemAtSlot(InAtItemPosition, 1);
		return;
	}
	
	UObsidianInventoryItemInstance* ItemInstance = PlayerStashComp->GetItemInstanceFromTabAtPosition(InAtItemPosition);
	if(ItemInstance->IsStackable() == false)
	{
		return;
	}
	
	const int32 CurrentItemStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	if(CurrentItemStacks <= 1)
	{
		return;
	}
	
	RemoveItemUIElements(EObsidianPanelOwner::PlayerStash);

	checkf(UnstackSliderClass, TEXT("Tried to create widget without valid widget class in fill it in"
								 " UObInventoryItemsWidgetController instance."));
	ActiveUnstackSlider = CreateWidget<UObsidianUnstackSlider>(OwnerPlayerController.Get(), UnstackSliderClass);
	ActiveUnstackSlider->InitializeUnstackSlider(CurrentItemStacks, InAtItemPosition);

	const FVector2D UnstackSliderViewportPosition = CalculateUnstackSliderPosition(InItemWidget);
	ActiveUnstackSlider->SetPositionInViewport(UnstackSliderViewportPosition);
	ActiveUnstackSlider->AddToViewport();
	bUnstackSliderActive = true;
	
	ActiveUnstackSlider->OnAcceptButtonPressedDelegate.AddUObject(this, &ThisClass::HandleTakingOutStacksFromStash);
	ActiveUnstackSlider->OnCloseButtonPressedDelegate.AddUObject(this, &ThisClass::RemoveUnstackSlider);
}

void UObInventoryItemsWidgetController::HandleRightClickingOnInventoryItem(const FIntPoint& InAtGridSlot,
	UObsidianItem* InItemWidget)
{
	UObsidianCraftingComponent* CraftingComp = OwnerCraftingComponent.Get();
	if (CraftingComp == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Trying to trigger crafting without valid Crafting Comp"
												" in [%hs]"), __FUNCTION__);
		return;
	}

	check(OwnerItemManagerComponent.IsValid());
	if (OwnerItemManagerComponent.IsValid() && OwnerItemManagerComponent.Get()->IsDraggingAnItem())
	{
		return;
	}
	
	check(OwnerInventoryComponent.IsValid());
	UObsidianInventoryComponent* InventoryComp = OwnerInventoryComponent.Get();
	if (InventoryComp == nullptr)
	{
		return;
	}

	if (InventoryComp->CanOwnerModifyInventoryState() == false)
	{
		return;
	}

	const UObsidianPlayerStashComponent* PlayerStashComp = OwnerPlayerStashComponent.Get();
	const UObsidianEquipmentComponent* EquipmentComp = OwnerEquipmentComponent.Get();
	if (PlayerStashComp == nullptr || EquipmentComp == nullptr)
	{
		return;
	}
	
	UObsidianInventoryItemInstance* UsingInstance = InventoryComp->GetItemInstanceAtLocation(InAtGridSlot);
	if(UsingInstance && UsingInstance->IsItemUsable() == false)
	{
		return;
	}

	if(UsingInstance->GetUsableItemType() == EObsidianUsableItemType::UIT_Crafting)
	{
		if (InItemWidget == nullptr)
		{
			UE_LOG(ObLogUIItems, Error, TEXT("ItemWidget is invalid in [%hs]."), __FUNCTION__);
			return;
		}
		
		CraftingComp->SetUsingItem(true, InItemWidget, UsingInstance);

		// This Whole thing needs to be multithreaded I think
		TArray<UObsidianInventoryItemInstance*> AllItems;
		AllItems.Append(InventoryComp->GetAllItems());
		AllItems.Append(EquipmentComp->GetAllEquippedItems());
		AllItems.Append(PlayerStashComp->GetAllItems());
		
		FObsidianItemsMatchingUsableContext MatchingUsableContext;
		if (UsingInstance->FireItemUseUIContext(AllItems, MatchingUsableContext))
		{
			if (MatchingUsableContext.InventoryItemsMatchingContext.IsEmpty() == false)
			{
				OnUsableContextFiredForInventoryDelegate.Broadcast(MatchingUsableContext.InventoryItemsMatchingContext);
			}
			if (MatchingUsableContext.EquipmentItemsMatchingContext.IsEmpty() == false)
			{
				OnUsableContextFiredForEquipmentDelegate.Broadcast(MatchingUsableContext.EquipmentItemsMatchingContext);
			}
			if (MatchingUsableContext.StashItemsMatchingContext.IsEmpty() == false)
			{
				OnUsableContextFiredForStashDelegate.Broadcast(MatchingUsableContext.StashItemsMatchingContext);
			}
		}
	}
	else if(UsingInstance->GetUsableItemType() == EObsidianUsableItemType::UIT_Activation)
	{
		CraftingComp->ServerActivateUsableItemFromInventory(UsingInstance);
	}
}

void UObInventoryItemsWidgetController::HandleRightClickingOnStashedItem(const FObsidianItemPosition& InAtItemPosition,
	UObsidianItem* InItemWidget)
{
	UObsidianCraftingComponent* CraftingComp = OwnerCraftingComponent.Get();
	if (CraftingComp == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Trying to trigger crafting without valid Crafting Comp"
												" in [%hs]"), __FUNCTION__);
		return;
	}
	
	check(OwnerItemManagerComponent.IsValid());
	if (OwnerItemManagerComponent.IsValid() && OwnerItemManagerComponent.Get()->IsDraggingAnItem())
	{
		return;
	}
	
	check(OwnerPlayerStashComponent.IsValid());
	UObsidianPlayerStashComponent* PlayerStashComp = OwnerPlayerStashComponent.Get();
	if (PlayerStashComp == nullptr)
	{
		return;
	}
	
	if (PlayerStashComp->CanOwnerModifyPlayerStashState() == false)
	{
		return;
	}

	check(OwnerInventoryComponent.IsValid());
	const UObsidianInventoryComponent* InventoryComp = OwnerInventoryComponent.Get();
	check(OwnerEquipmentComponent.IsValid());
	const UObsidianEquipmentComponent* EquipmentComp = OwnerEquipmentComponent.Get();
	if (InventoryComp == nullptr || EquipmentComp == nullptr)
	{
		return;
	}
	
	UObsidianInventoryItemInstance* UsingInstance = PlayerStashComp->GetItemInstanceFromTabAtPosition(
		InAtItemPosition);
	if(UsingInstance && UsingInstance->IsItemUsable() == false)
	{
		return;
	}

	if(UsingInstance->GetUsableItemType() == EObsidianUsableItemType::UIT_Crafting)
	{
		if (InItemWidget == nullptr)
		{
			UE_LOG(ObLogUIItems, Error, TEXT("ItemWidget is invalid in [%hs]."), __FUNCTION__);
			return;
		}
		
		CraftingComp->SetUsingItem(true, InItemWidget, UsingInstance);

		// This Whole thing needs to be multithreaded I think
		TArray<UObsidianInventoryItemInstance*> AllItems;
		AllItems.Append(InventoryComp->GetAllItems());
		AllItems.Append(EquipmentComp->GetAllEquippedItems());
		AllItems.Append(PlayerStashComp->GetAllItems());
	
		FObsidianItemsMatchingUsableContext MatchingUsableContext;
		if (UsingInstance->FireItemUseUIContext(AllItems, MatchingUsableContext))
		{
			if (MatchingUsableContext.InventoryItemsMatchingContext.IsEmpty() == false)
			{
				OnUsableContextFiredForInventoryDelegate.Broadcast(MatchingUsableContext.InventoryItemsMatchingContext);
			}
			if (MatchingUsableContext.EquipmentItemsMatchingContext.IsEmpty() == false)
			{
				OnUsableContextFiredForEquipmentDelegate.Broadcast(MatchingUsableContext.EquipmentItemsMatchingContext);
			}
			if (MatchingUsableContext.StashItemsMatchingContext.IsEmpty() == false)
			{
				OnUsableContextFiredForStashDelegate.Broadcast(MatchingUsableContext.StashItemsMatchingContext);
			}
		}
	}
	else if(UsingInstance->GetUsableItemType() == EObsidianUsableItemType::UIT_Activation)
	{
		CraftingComp->ServerActivateUsableItemFromStash(UsingInstance);
	}
}

void UObInventoryItemsWidgetController::OnStartDraggingItem(const FDraggedItem& InDraggedItem)
{
	if (OwnerPlayerController == nullptr || InDraggedItem.IsEmpty())
	{
		return;
	}
	
	const AObsidianHUD* ObsidianHUD = OwnerPlayerController->GetObsidianHUD();
	if (ObsidianHUD == nullptr)
	{
		return;
	}
	
	FGameplayTagContainer JoinedSlotTags;

	UObsidianEquipmentComponent* EquipmentComp = OwnerEquipmentComponent.Get();
	if (EquipmentComp && ObsidianHUD->IsInventoryOpened()) // Gather possible equipment slots
	{
		if (const UObsidianInventoryItemInstance* DraggedInstance = InDraggedItem.Instance)
		{
			for (const FObsidianEquipmentSlotDefinition& EquipmentSlot : EquipmentComp->FindPossibleSlotsForEquipping_WithInstance(
				DraggedInstance))
			{
				JoinedSlotTags.AddTag(EquipmentSlot.GetEquipmentSlotTag());
			}
		}
		else if (const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef = InDraggedItem.ItemDef)
		{
			for (const FObsidianEquipmentSlotDefinition& EquipmentSlot : EquipmentComp->FindPossibleSlotsForEquipping_WithItemDef(
				ItemDef, InDraggedItem.GeneratedData))
			{
				JoinedSlotTags.AddTag(EquipmentSlot.GetEquipmentSlotTag());
			}
		}
	}

	UObsidianPlayerStashComponent* StashComp = OwnerPlayerStashComponent.Get();
	if (StashComp && ObsidianHUD->IsPlayerStashOpened()) // Gather possible functional slots
	{
		if (const UObsidianInventoryItemInstance* DraggedInstance = InDraggedItem.Instance)
		{
			for (const FObsidianStashSlotDefinition& StashSlot : StashComp->FindPossibleSlotsForPlacingItem_WithInstance(
				DraggedInstance))
			{
				JoinedSlotTags.AddTag(StashSlot.GetStashSlotTag());
			}
		}
		else if (const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef = InDraggedItem.ItemDef)
		{
			for (const FObsidianStashSlotDefinition& StashSlot : StashComp->FindPossibleSlotsForPlacingItem_WithItemDef(
				ItemDef))
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

void UObInventoryItemsWidgetController::HandleTakingOutStacksFromInventory(const int32 InStacksToTake,
	const FObsidianItemPosition& InItemPosition)
{
	RemoveUnstackSlider();

	check(OwnerItemManagerComponent.IsValid());
	UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get();
	if (ItemManager == nullptr)
	{
		return;
	}
	
	if(InStacksToTake == 0 || OwnerInventoryComponent == nullptr)
	{
		return;
	}

	const FIntPoint GridPosition = InItemPosition.GetItemGridPosition();
	
	if(const UObsidianInventoryItemInstance* Instance = OwnerInventoryComponent->GetItemInstanceAtLocation(
		GridPosition))
	{
		if(Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current) == InStacksToTake)
		{
			ItemManager->ServerGrabInventoryItemToCursor(GridPosition);
			return;
		}
		ItemManager->ServerTakeoutFromInventoryItem(GridPosition, InStacksToTake);
	}
}

void UObInventoryItemsWidgetController::HandleTakingOutStacksFromStash(const int32 InStacksToTake,
	const FObsidianItemPosition& InItemPosition)
{
	RemoveUnstackSlider();

	check(OwnerItemManagerComponent.IsValid());
	UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get();
	if (ItemManager == nullptr)
	{
		return;
	}
	
	if(InStacksToTake == 0 || OwnerPlayerStashComponent == nullptr)
	{
		return;
	}
	
	if(const UObsidianInventoryItemInstance* Instance = OwnerPlayerStashComponent->GetItemInstanceFromTabAtPosition(
		InItemPosition))
	{
		if(Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current) == InStacksToTake)
		{
			ItemManager->ServerGrabStashedItemToCursor(InItemPosition);
			return;
		}
		ItemManager->ServerTakeoutFromStashedItem(InItemPosition, InStacksToTake);
	}
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

void UObInventoryItemsWidgetController::ClearItemDescriptionForPosition(const FObsidianItemPosition& InForPosition)
{
	FObsidianActiveItemDescriptionData DescriptionData;
	if (ActiveItemDescriptions.RemoveAndCopyValue(InForPosition, DescriptionData))
	{
		if (UObsidianItemDescriptionBase* DescriptionWidget = DescriptionData.OwningItemDescription)
		{
			DescriptionWidget->DestroyDescriptionWidget();
		}
	}
}

void UObInventoryItemsWidgetController::ClearItemDescriptionsForOwner(const EObsidianPanelOwner InForDescriptionOwner)
{
	for(auto It = ActiveItemDescriptions.CreateIterator(); It; ++It)
	{
		auto& Entry = *It;
		if(Entry.Value.DescriptionPanelOwner == InForDescriptionOwner)
		{
			if (UObsidianItemDescriptionBase* DescriptionWidget = Entry.Value.OwningItemDescription)
			{
				DescriptionWidget->DestroyDescriptionWidget();
				It.RemoveCurrent();
			}
		}
	}
}

UObsidianItemDescriptionBase* UObInventoryItemsWidgetController::CreateInventoryItemDescription(
	const FObsidianItemPosition& InAtPosition, const EObsidianPanelOwner InPanelOwner, const UObsidianItem* InForItemWidget,
	const FObsidianItemStats& InItemStats)
{
	ClearItemDescriptionForPosition(InAtPosition); //TODO(intrxx) will it be necessary?
	
	checkf(ItemDescriptionClass, TEXT("Tried to create widget without valid widget class, fill it in "
								   "UObInventoryItemsWidgetController instance."));
	UObsidianItemDescriptionBase* NewItemDescription = CreateWidget<UObsidianItemDescriptionBase>(
		OwnerPlayerController.Get(), ItemDescriptionClass);
	NewItemDescription->InitializeWidgetWithItemStats(InItemStats);
	NewItemDescription->AddToViewport();
	
	const FVector2D DescriptionViewportPosition = CalculateDescriptionPosition(InForItemWidget, NewItemDescription);
	NewItemDescription->SetPositionInViewport(DescriptionViewportPosition);
	
	ActiveItemDescriptions.Add(InAtPosition, FObsidianActiveItemDescriptionData(NewItemDescription, InPanelOwner));
	
	return NewItemDescription;
}

UObsidianItemDescriptionBase* UObInventoryItemsWidgetController::CreateDroppedItemDescription(
	const FObsidianItemStats& InItemStats)
{
	check(OwnerPlayerController.IsValid());
	const AObsidianPlayerController* ObsidianPC = OwnerPlayerController.Get();
	if (ObsidianPC == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("ObsidianPC is invalid in [%hs]."), __FUNCTION__);
		return nullptr;
	}
	
	AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD();
	if(ObsidianHUD == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Unable to get ObsidianHUD in [%hs]."), __FUNCTION__);
		return nullptr;
	}

	const UObsidianMainOverlay* MainOverlay = ObsidianHUD->GetMainOverlay();
	if(MainOverlay == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Unable to get ObsidianMainOverlay in [%hs]."),
			__FUNCTION__);
		return nullptr;
	}

	RemoveCurrentDroppedItemDescription(); // Clear any other Item Description

	checkf(ItemDescriptionClass, TEXT("Tried to create widget without valid widget class,"
								   " fill it in UObInventoryItemsWidgetController instance."));
	ActiveDroppedItemDescription = CreateWidget<UObsidianItemDescriptionBase>(OwnerPlayerController.Get(),
		ItemDescriptionClass);
	ActiveDroppedItemDescription->InitializeWidgetWithItemStats(InItemStats, true);
	MainOverlay->AddItemDescriptionToOverlay(ActiveDroppedItemDescription);
	bDroppedDescriptionActive = true;
	
	return ActiveDroppedItemDescription;
}

FVector2D UObInventoryItemsWidgetController::CalculateUnstackSliderPosition(const UObsidianItem* InItemWidget) const
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Failed to calculate Unstack Slider Position"));
		return FVector2D::Zero();
	}

	if(InItemWidget == nullptr || ActiveUnstackSlider == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Failed to calculate Unstack Slider Position"));
		return FVector2D::Zero();
	}
	
	FVector2D SliderSize = ActiveUnstackSlider->GetSizeBoxSize();
		
	const FGeometry& CachedGeometry = InItemWidget->GetCachedGeometry();
	FVector2D ItemLocalSize = InItemWidget->GetItemWidgetSize();

	// Adjusting sizes based on viewport scale
	const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(World);
	SliderSize *= DPIScale; 
	ItemLocalSize *= DPIScale;

	FVector2D ItemPixelPosition = FVector2D::Zero();
	FVector2D ItemViewportPosition = FVector2D::Zero();
	USlateBlueprintLibrary::LocalToViewport(World, CachedGeometry, FVector2D(0.f,0.f),
		ItemPixelPosition, ItemViewportPosition);

	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(World);
	return GetItemUIElementPositionBoundByViewport(ViewportSize, ItemPixelPosition, ItemLocalSize,
		SliderSize);
}

FVector2D UObInventoryItemsWidgetController::CalculateDescriptionPosition(const UObsidianItem* InItemWidget,
	UObsidianItemDescriptionBase* InForDescription) const
{
	const UWorld* World = GetWorld();
	if(World == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Failed to calculate Description Position"));
		return FVector2D::Zero();
	}

	if(InItemWidget == nullptr || InForDescription == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("Failed to calculate Description Position"));
		return FVector2D::Zero();
	}

	// @HACK this is quite ugly, but without prepass the desired size is [0, 0], if the performance is the problem,
	// I could delay the calculation for a frame and see how reliable it is to retrieve the sie information,
	// Other system with delegates could be implemented to get the size reliably, but it just needs testing cuz if it's not bad I don't really care for now.
	InForDescription->ForceLayoutPrepass();
	FVector2D DescriptionSize = InForDescription->GetDesiredSize();
		
	const FGeometry& CachedGeometry = InItemWidget->GetCachedGeometry();
	FVector2D ItemLocalSize = InItemWidget->GetItemWidgetSize();
		
	// Adjusting sizes based on viewport scale
	const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(World);
	ItemLocalSize *= DPIScale; 
	DescriptionSize *= DPIScale;
		
	FVector2D ItemPixelPosition = FVector2D::Zero();
	FVector2D ItemViewportPosition = FVector2D::Zero();
	USlateBlueprintLibrary::LocalToViewport(World, CachedGeometry, FVector2D(0.f,0.f),
		ItemPixelPosition, ItemViewportPosition);
	
	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(World);
	return GetItemUIElementPositionBoundByViewport(ViewportSize, ItemPixelPosition, ItemLocalSize,
		DescriptionSize);
}

FVector2D UObInventoryItemsWidgetController::GetItemUIElementPositionBoundByViewport(const FVector2D& InViewportSize,
	const FVector2D& InItemPosition, const FVector2D& InItemSize, const FVector2D& InUIElementSize) const
{
	FVector2D BoundedPosition = FVector2D((InItemPosition.X - (InUIElementSize.X / 2)) + (InItemSize.X / 2),
										  (InItemPosition.Y - InUIElementSize.Y));
	
	const bool bFitsLeft = BoundedPosition.X > 0.0f;
	const bool bFitsRight = (BoundedPosition.X + InUIElementSize.X) < InViewportSize.X;
	const bool bFitsTop = (BoundedPosition.Y - InUIElementSize.Y) > 0.0f;
	
	if(bFitsLeft && bFitsRight && bFitsTop) // We fit in the default position [top-middle]
	{
		return BoundedPosition;
	}
	
	if(bFitsLeft && bFitsRight && bFitsTop == false)
	{
		BoundedPosition = FVector2D(
			(InItemPosition.X - (InUIElementSize.X / 2)) + (InItemSize.X / 2),
			(InItemPosition.Y + InItemSize.Y));
		if((BoundedPosition.Y + InUIElementSize.Y) < InViewportSize.Y) // Desc fit below [bottom-middle]
		{
			return BoundedPosition;
		}
	}
	
	if(bFitsRight == false)
	{
		BoundedPosition = FVector2D(
			(InItemPosition.X - InUIElementSize.X),
			(InItemPosition.Y - (InUIElementSize.Y / 2)) + (InItemSize.Y / 2));
		if(BoundedPosition.X > 0.0f) // Desc fit left [left-middle]
		{
			return BoundedPosition;
		}
	}
	
	if(bFitsLeft == false)
	{
		BoundedPosition = FVector2D(
			(InItemPosition.X + InItemSize.X),
			(InItemPosition.Y - (InUIElementSize.Y / 2)) + (InItemSize.Y / 2));
		if((BoundedPosition.X + InUIElementSize.X) < InViewportSize.X) // Desc Fit right [right-middle]
		{
			return BoundedPosition;
		}
	}
	
	// Falling back to the default not so happy position which is most likely to fit if all above cases fail,
	// could improve it later to fit the screen in every case but is not necessary now [middle-middle].
	BoundedPosition = FVector2D(
		(InItemPosition.X - (InUIElementSize.X / 2)) + (InItemSize.X / 2),
		(InItemPosition.Y - (InUIElementSize.Y / 2)) + (InItemSize.Y / 2));
	
	return BoundedPosition;
}



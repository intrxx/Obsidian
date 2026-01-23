// Copyright 2026 out of sCope team - intrxx

#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"

#include <GameFramework/GameplayMessageSubsystem.h>
#include <Blueprint/SlateBlueprintLibrary.h>
#include <Blueprint/WidgetLayoutLibrary.h>

#include "UI/Inventory/Items/ObsidianItemDescriptionBase.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemManagerComponent.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "InventoryItems/Crafting/ObsidianCraftingComponent.h"
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
	check(OwnerPlayerController.IsValid());
	const AObsidianPlayerController* PlayerController = OwnerPlayerController.Get();
	if (PlayerController == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("PlayerController is invalid in [%hs]."), __FUNCTION__);
	}
	
	OwnerCraftingComponent = PlayerController->GetCraftingComponent();
	check(OwnerCraftingComponent.IsValid());
	if (UObsidianCraftingComponent* CraftingComp = OwnerCraftingComponent.Get())
	{
		CraftingComp->OnStopUsingItemDelegate.AddUObject(this, &ThisClass::ClearUsableUIContext);
	}

	OwnerInventoryComponent = PlayerController->GetInventoryComponent();
	check(OwnerInventoryComponent.IsValid())
	OwnerEquipmentComponent = PlayerController->GetEquipmentComponent();
	check(OwnerEquipmentComponent.IsValid())
	OwnerPlayerStashComponent = PlayerController->GetPlayerStashComponent();
	check(OwnerPlayerStashComponent.IsValid())

	const AActor* OwningActor = Cast<AActor>(OwnerPlayerController->GetPawn());
	check(OwningActor);
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwningActor->GetWorld());
	MessageSubsystem.RegisterListener(ObsidianGameplayTags::Message_Inventory_Changed, this,
		&ThisClass::OnInventoryStateChanged);
	MessageSubsystem.RegisterListener(ObsidianGameplayTags::Message_Equipment_Changed, this,
		&ThisClass::OnEquipmentStateChanged);
	MessageSubsystem.RegisterListener(ObsidianGameplayTags::Message_PlayerStash_Changed, this,
		&ThisClass::OnPlayerStashChanged);
	
	OwnerItemManagerComponent = OwnerPlayerController->GetItemManagerComponent();
	check(OwnerItemManagerComponent.IsValid());
	if (UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get())
	{
		ItemManager->OnStartDraggingItemDelegate.AddUObject(this, &ThisClass::OnStartDraggingItem);
		ItemManager->OnStopDraggingItemDelegate.AddUObject(this, &ThisClass::OnStopDraggingItem);
	}
}

void UObInventoryItemsWidgetController::OnInventoryStateChanged(FGameplayTag Channel,
	const FObsidianInventoryChangeMessage& InventoryChangeMessage)
{
	// Fixes a bug when Items appear in Server's Inventory (Listen Server Character) after picked up by client.
	if(OwnerInventoryComponent != InventoryChangeMessage.InventoryOwner)
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
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Adding item: [%s] to Inventory"),
			*Instance->GetItemDisplayName().ToString());
		
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
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Removing item: [%s] from Inventory"),
			*Instance->GetItemDisplayName().ToString());
		
		ClearItemDescriptionForPosition(InventoryChangeMessage.GridItemPosition);

		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemPosition = InventoryChangeMessage.GridItemPosition;
		OnInventorizedItemRemovedDelegate.Broadcast(ItemWidgetData);
	}
	else if (InventoryChangeMessage.ChangeType == EObsidianInventoryChangeType::ICT_ItemStacksChanged)
	{
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Changing item: [%s] in Inventory"),
			*Instance->GetItemDisplayName().ToString());
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemPosition = InventoryChangeMessage.GridItemPosition;
		ItemWidgetData.StackCount = Instance->IsStackable() ? InventoryChangeMessage.NewCount : 0;
		
		OnInventoryItemChangedDelegate.Broadcast(ItemWidgetData);
	}
}

void UObInventoryItemsWidgetController::OnEquipmentStateChanged(FGameplayTag Channel,
	const FObsidianEquipmentChangeMessage& EquipmentChangeMessage)
{
	// Fixes a bug when Items appear in Server's Inventory (Listen Server Character) after picked up by client.
	if(OwnerEquipmentComponent != EquipmentChangeMessage.EquipmentOwner) 
	{
		return;
	}
	
	const UObsidianInventoryItemInstance* Instance = EquipmentChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Inventory Item Instance is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}

	if(EquipmentChangeMessage.ChangeType == EObsidianEquipmentChangeType::ECT_ItemEquipped)
	{
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Equipping item: [%s]"),
			*Instance->GetItemDisplayName().ToString());

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
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Unequipping item: [%s]"),
			*Instance->GetItemDisplayName().ToString());
		
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
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Equipment Swapping item: [%s]"),
			*Instance->GetItemDisplayName().ToString());

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
	// Fixes a bug when Items appear in Server's Inventory (Listen Server Character) after picked up by client.
	if(OwnerPlayerStashComponent != StashChangeMessage.PlayerStashOwner)
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
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Adding item: [%s] to Player Stash"),
			*Instance->GetItemDisplayName().ToString());
		
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
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Removing item: [%s] from Player Stash"),
			*Instance->GetItemDisplayName().ToString());
		//RemoveStashItemWidget(StashChangeMessage.ItemPosition);

		ClearItemDescriptionForPosition(StashChangeMessage.ItemPosition);
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemPosition = StashChangeMessage.ItemPosition;
		OnStashedItemRemovedDelegate.Broadcast(ItemWidgetData);
	}
	else if (StashChangeMessage.ChangeType == EObsidianStashChangeType::ICT_ItemStacksChanged)
	{
		UE_LOG(LogWidgetController_Items, Display, TEXT("[Widget] Changing item: [%s] in Player Stash"),
			*Instance->GetItemDisplayName().ToString());
		
		FObsidianItemWidgetData ItemWidgetData;
		ItemWidgetData.ItemPosition = StashChangeMessage.ItemPosition;
		ItemWidgetData.StackCount = Instance->IsStackable() ? StashChangeMessage.NewCount : 0;
		
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
										Item->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current) :
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
	UE_LOG(LogTemp, Display, TEXT("Stash Opened."));

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
										Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current) :
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
	
	UE_LOG(LogWidgetController_Items, Error, TEXT("Trying to GetAllStashTabs but Stash Component is invalid in"
											   " [%hs]"), __FUNCTION__);
	return {};	
}

FString UObInventoryItemsWidgetController::GetStashTabName(const FGameplayTag StashTabTag) const
{
	if (UObsidianPlayerStashComponent* PlayerStashComp = OwnerPlayerStashComponent.Get())
	{
		if (const UObsidianStashTab* StashTab = PlayerStashComp->GetStashTabForTag(StashTabTag))
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
	
	UE_LOG(LogWidgetController_Items, Error, TEXT("Trying to return Grid Width but Inventory Component is"
											   " invalid in [%hs]"), __FUNCTION__);
	return 0;
}

int32 UObInventoryItemsWidgetController::GetInventoryGridHeight() const
{
	if (const UObsidianInventoryComponent* InventoryComp = OwnerInventoryComponent.Get())
	{
		return InventoryComp->GetInventoryGridHeight();
	}
	
	UE_LOG(LogWidgetController_Items, Error, TEXT("Trying to return Grid Height but Inventory Component is"
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

FIntPoint UObInventoryItemsWidgetController::GetItemGridSpanByPosition(const FObsidianItemPosition& ItemPosition) const
{
	if (ItemPosition.IsValid() == false)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Trying to retrieve Item Grid Span from invalid Position."));
		return FIntPoint::NoneValue; 
	}

	if (ItemPosition.IsOnInventoryGrid())
	{
		if (const UObsidianInventoryItemInstance* Instance = OwnerInventoryComponent->GetItemInstanceAtLocation(
			ItemPosition.GetItemGridPosition()))
		{
			return Instance->GetItemGridSpan();
		}
	}
	else if (ItemPosition.IsOnStash())
	{
		if (const UObsidianInventoryItemInstance* Instance = OwnerPlayerStashComponent->GetItemInstanceFromTabAtPosition(
			ItemPosition))
		{
			return Instance->GetItemGridSpan();
		}
	}
	else if (ItemPosition.IsOnEquipmentSlot())
	{
		if (const UObsidianInventoryItemInstance* Instance = OwnerEquipmentComponent->GetEquippedInstanceAtSlot(
			ItemPosition.GetItemSlotTag()))
		{
			return Instance->GetItemGridSpan();
		}
	}
	
	UE_LOG(LogWidgetController_Items, Error, TEXT("Was unable to retrieve Item Grid Span from invalid Position."));
	return FIntPoint::NoneValue;
}

bool UObInventoryItemsWidgetController::CanInteractWithGrid(const EObsidianPanelOwner PanelOwner) const
{
	switch (PanelOwner)
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
		UE_LOG(LogWidgetController_Items, Error, TEXT("There is no valid PanelOwner in [%hs]"), __FUNCTION__);
		break;
	}
	return false;
}

bool UObInventoryItemsWidgetController::CanInteractWithSlots(const EObsidianPanelOwner PanelOwner) const
{
	switch (PanelOwner)
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
			return OwnerPlayerStashComponent->CanOwnerModifyPlayerStashState();
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
					ensureMsgf(InteractionData.InteractionTargetPositionOverride.IsOnInventoryGrid(),
						TEXT("Trying to handle left click on Inventorized item with invalid Interaction"
						" Target Position Override."));
					
					HandleLeftClickingOnInventoryItem(AtItemPosition.GetItemGridPosition(),
						InteractionData.InteractionTargetPositionOverride.GetItemGridPosition(),
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
				
			ForInstance = OwnerInventoryComponent->GetItemInstanceAtLocation(ItemPosition.GetItemGridPosition());
			break;
		}
	case EObsidianPanelOwner::Equipment:
		{
			ensureMsgf(ItemPosition.IsOnEquipmentSlot(), TEXT("Trying to hover over Equipped Item with"
													" invalid Equipment position."));
				
			ForInstance = OwnerEquipmentComponent->GetEquippedInstanceAtSlot(ItemPosition.GetItemSlotTag());
			break;
		}
	case EObsidianPanelOwner::PlayerStash:
		{
			ensureMsgf(ItemPosition.IsOnStash(), TEXT("Trying to hover over Stashed Item with"
													" invalid Stash position."));
				
			ForInstance = OwnerPlayerStashComponent->GetItemInstanceFromTabAtPosition(ItemPosition);
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
	if (UObsidianItemsFunctionLibrary::GetItemStats(OwnerPlayerController.Get(), ForInstance, OutItemStats))
	{
		CreateInventoryItemDescription(ItemPosition, PanelOwner, InteractionData.ItemWidget, OutItemStats);
	}
}

void UObInventoryItemsWidgetController::HandleUnhoveringItem(const FObsidianItemPosition& FromPosition)
{
	ClearItemDescriptionForPosition(FromPosition);
}

void UObInventoryItemsWidgetController::RemoveItemUIElements(const EObsidianPanelOwner ForPanelOwner)
{
	ClearItemDescriptionsForOwner(ForPanelOwner);
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

void UObInventoryItemsWidgetController::CreateItemDescriptionForDroppedItem(const UObsidianInventoryItemInstance* Instance)
{
	if(CanShowDescription() == false)
	{
		return;
	}
	
	FObsidianItemStats OutItemStats;
	if(UObsidianItemsFunctionLibrary::GetItemStats(OwnerPlayerController.Get(), Instance, OutItemStats))
	{
		CreateDroppedItemDescription(OutItemStats);
	}
}

void UObInventoryItemsWidgetController::CreateItemDescriptionForDroppedItem(
	const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FObsidianItemGeneratedData& ItemGeneratedData)
{
	if(!CanShowDescription())
	{
		return;
	}
	
	FObsidianItemStats OutItemStats;
	if(UObsidianItemsFunctionLibrary::GetItemStats_WithDef(OwnerPlayerController.Get(), ItemDef,
		ItemGeneratedData, OutItemStats))
	{
		CreateDroppedItemDescription(OutItemStats);
	}
}

void UObInventoryItemsWidgetController::RegisterCurrentStashTab(const FGameplayTag& CurrentStashTab)
{
	if (UObsidianPlayerStashComponent* PlayerStashComp = OwnerPlayerStashComponent.Get())
	{
		PlayerStashComp->ServerRegisterAndValidateCurrentStashTab(CurrentStashTab);
	}
	else
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Tried to register Current Stash Tab but the"
												" OwnerPlayerStashComponent is invalid in [%hs]"), __FUNCTION__);
	}
}

bool UObInventoryItemsWidgetController::CanPlaceDraggedItemInInventory(const FIntPoint& AtGridSlot) const
{
	if(OwnerInventoryComponent == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("OwnerPlayerInputManager or OwnerInventoryComponent is"
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
	
	return OwnerInventoryComponent->CheckSpecifiedPosition(DraggedItemGridSpan, AtGridSlot);
}

bool UObInventoryItemsWidgetController::CanPlaceDraggedItemInStash(const FObsidianItemPosition& ItemPosition) const
{
	if (OwnerPlayerStashComponent == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("OwnerPlayerInputManager or OwnerPlayerStashComponent is"
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
	
	return OwnerPlayerStashComponent->CheckSpecifiedPosition(ItemPosition, CategoryTag, ItemBaseType, DraggedItemGridSpan);
}

bool UObInventoryItemsWidgetController::CanPlaceDraggedItemInEquipment(const FGameplayTag& SlotTag) const
{
	check(OwnerItemManagerComponent.IsValid());
	UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get();
	if (ItemManager == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("InputManager is invalid in [%hs]."), __FUNCTION__);
		return false;
	}

	check(OwnerEquipmentComponent.IsValid());
	UObsidianEquipmentComponent* EquipmentComp = OwnerEquipmentComponent.Get();
	if (EquipmentComp == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("EquipmentComp is invalid in [%hs]."), __FUNCTION__);
		return false; 
	}

	const bool bSlotOccupied = EquipmentComp->IsItemEquippedAtSlot(SlotTag);
	const FDraggedItem DraggedItem = ItemManager->GetDraggedItem();
	if(const UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance)
	{
		EObsidianEquipCheckResult EquipResult;
		if(bSlotOccupied)
		{
			EquipResult	= EquipmentComp->CanReplaceInstance(DraggedInstance, SlotTag);
		}
		else
		{
			EquipResult	= EquipmentComp->CanEquipInstance(DraggedInstance, SlotTag);
		}
		 
		return EquipResult == EObsidianEquipCheckResult::CanEquip;
	}
	if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef)
	{
		EObsidianEquipCheckResult EquipResult;
		if(bSlotOccupied)
		{
			EquipResult = EquipmentComp->CanReplaceTemplate(DraggedItemDef, SlotTag, DraggedItem.GeneratedData);
		}
		else
		{
			EquipResult = EquipmentComp->CanEquipTemplate(DraggedItemDef, SlotTag, DraggedItem.GeneratedData);
		}
		
		return EquipResult == EObsidianEquipCheckResult::CanEquip;
	}
	return false;
}

bool UObInventoryItemsWidgetController::CanShowDescription() const
{
	return !bUnstackSliderActive;
}

void UObInventoryItemsWidgetController::RequestAddingItemToInventory(const FIntPoint& ToGridSlot, const bool bShiftDown)
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
	
	ItemManager->ServerAddItemToInventoryAtSlot(ToGridSlot, bShiftDown);
}

void UObInventoryItemsWidgetController::RequestAddingItemToEquipment(const FGameplayTag& SlotTag)
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
	
	ItemManager->ServerEquipItemAtSlot(SlotTag);
}

void UObInventoryItemsWidgetController::RequestAddingItemToStashTab(const FObsidianItemPosition& ToPosition,
	const bool bShiftDown)
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
	
	ItemManager->ServerAddItemToStashTabAtSlot(ToPosition, bShiftDown);
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnInventoryItem(const FIntPoint& ClickedItemPosition,
	const FIntPoint& ClickedGridPosition, const bool bAddToOtherWindow)
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
		CraftingComp->UseItem(ClickedItemPosition, false);
		return;
	}

	if (ItemManager->IsDraggingAnItem() == false)
	{
		if (bAddToOtherWindow == false)
		{
			ItemManager->ServerGrabInventoryItemToCursor(ClickedItemPosition);
			return;
		}

		AObsidianHUD* ObsidianHUD = OwnerPlayerController->GetObsidianHUD();
		if (ObsidianHUD && ObsidianHUD->IsPlayerStashOpened()) //TODO(intrxx) For now I support only Inventory <-> Stash
		{
			const FGameplayTag ToStashTab = ObsidianHUD->GetActiveStashTabTag(); //TODO(intrxx) This will need updating when I will support Stash Tab Affinities
			ItemManager->ServerTransferItemToPlayerStash(ClickedItemPosition, ToStashTab);
		}
		return;
	}
	
	const UObsidianInventoryItemInstance* InstanceToAddTo = InventoryComp->GetItemInstanceAtLocation(ClickedItemPosition);
	if (InstanceToAddTo == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Item Instance at pressed Location is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}
		
	const FDraggedItem DraggedItem = ItemManager->GetDraggedItem();
	if (UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance) // We carry item instance.
	{
		if (DraggedInstance->IsStackable() && UObsidianItemsFunctionLibrary::IsTheSameItem(DraggedInstance,
			InstanceToAddTo))
		{
			ItemManager->ServerAddStacksFromDraggedItemToInventoryItemAtSlot(ClickedItemPosition);
		}
		else if (InventoryComp->CanReplaceItemAtSpecificSlotWithInstance(ClickedItemPosition,
			ClickedGridPosition, DraggedInstance))
		{
			ItemManager->ServerReplaceItemAtInventorySlot(ClickedItemPosition, ClickedGridPosition);
		}
	}
	else if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef) // We carry item def
	{
		const UObsidianInventoryItemDefinition* DefaultObject = DraggedItemDef.GetDefaultObject();
		if (DefaultObject && DefaultObject->IsStackable() && UObsidianItemsFunctionLibrary::IsTheSameItem_WithDef(
																InstanceToAddTo, DraggedItemDef))
		{
			ItemManager->ServerAddStacksFromDraggedItemToInventoryItemAtSlot(ClickedItemPosition);
		}
		else if (InventoryComp->CanReplaceItemAtSpecificSlotWithDef(ClickedItemPosition,
			ClickedGridPosition, DraggedItemDef, DraggedItem.GeneratedData.AvailableStackCount))
		{
			ItemManager->ServerReplaceItemAtInventorySlot(ClickedItemPosition, ClickedGridPosition);
		}
	}
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnInventoryItemWithShiftDown(const FIntPoint& ClickedItemPosition,
	const UObsidianItem* ItemWidget)
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
		CraftingComp->UseItem(ClickedItemPosition, true);
		return;
	}
	
	if(ItemManager->IsDraggingAnItem())
	{
		ItemManager->ServerAddStacksFromDraggedItemToInventoryItemAtSlot(ClickedItemPosition, 1);
		return;
	}
	
	UObsidianInventoryItemInstance* ItemInstance = InventoryComp->GetItemInstanceAtLocation(ClickedItemPosition);
	if(ItemInstance->IsStackable() == false)
	{
		return;
	}
	
	const int32 CurrentItemStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(CurrentItemStacks <= 1)
	{
		return;
	}
	
	RemoveItemUIElements(EObsidianPanelOwner::Inventory);

	checkf(UnstackSliderClass, TEXT("Tried to create widget without valid widget class in fill it in "
								 "UObInventoryItemsWidgetController instance."));
	ActiveUnstackSlider = CreateWidget<UObsidianUnstackSlider>(OwnerPlayerController.Get(), UnstackSliderClass);
	ActiveUnstackSlider->InitializeUnstackSlider(CurrentItemStacks, ClickedItemPosition);

	const FVector2D UnstackSliderViewportPosition = CalculateUnstackSliderPosition(ItemWidget);
	ActiveUnstackSlider->SetPositionInViewport(UnstackSliderViewportPosition);
	ActiveUnstackSlider->AddToViewport();
	bUnstackSliderActive = true;
	
	ActiveUnstackSlider->OnAcceptButtonPressedDelegate.AddUObject(this, &ThisClass::HandleTakingOutStacksFromInventory);
	ActiveUnstackSlider->OnCloseButtonPressedDelegate.AddUObject(this, &ThisClass::RemoveUnstackSlider);
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnEquipmentItem(const FGameplayTag& SlotTag,
	const FGameplayTag& EquipSlotTagOverride)
{
	const FGameplayTag SwapSlotTag = FGameplayTag::RequestGameplayTag(TEXT("Item.SwapSlot.Equipment"));
	if(SlotTag.MatchesTag(SwapSlotTag))
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
		UE_LOG(LogWidgetController_Items, Error, TEXT("As of now it is impossible to use items onto equipped items,"
												" maybe will support it in the future."));
		return;
	}
	
	if(ItemManager->IsDraggingAnItem()) // If we carry an item, try to replace it with it.
	{
		
		 const FDraggedItem DraggedItem = ItemManager->GetDraggedItem();
		 if(UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance) // We carry item instance.
		 {
		 	const EObsidianEquipCheckResult EquipmentResult = EquipmentComp->CanReplaceInstance(DraggedInstance, SlotTag);
		 	if(EquipmentResult == EObsidianEquipCheckResult::CanEquip)
		 	{
		 		ItemManager->ServerReplaceItemAtEquipmentSlot(SlotTag, EquipSlotTagOverride);
		 	}
		 	else
		 	{
		 		//TODO(intrxx) Send Client RPC with some voice over passing EquipResult?
#if !UE_BUILD_SHIPPING
		 		UE_LOG(LogEquipment, Warning, TEXT("Item cannot be equipped, reason: [%s]"),
		 			*ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipmentResult));
#endif
		 	}
		 	return;
		 }
		
		 if(const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef) // We carry item def
		 {
		 	const EObsidianEquipCheckResult EquipmentResult = EquipmentComp->CanReplaceTemplate(DraggedItemDef, SlotTag,
		 		DraggedItem.GeneratedData);
		 	if(EquipmentResult == EObsidianEquipCheckResult::CanEquip)
		 	{
		 		ItemManager->ServerReplaceItemAtEquipmentSlot(SlotTag, EquipSlotTagOverride);
		 	}
		 	else
		 	{
		 		//TODO(intrxx) Send Client RPC with some voice over passing EquipResult?
#if !UE_BUILD_SHIPPING
			 	UE_LOG(LogEquipment, Warning, TEXT("Item cannot be equipped, reason: [%s]"),
			 		*ObsidianEquipmentDebugHelpers::GetEquipResultString(EquipmentResult));
#endif
			 }
			 return;
		}
		return;
	}
	ItemManager->ServerGrabEquippedItemToCursor(SlotTag);
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnStashedItem(const FObsidianItemPosition& AtItemPosition,
	const bool bAddToOtherWindow)
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
		CraftingComp->UseItem(AtItemPosition, false);
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
		if (bAddToOtherWindow == false)
		{
			ItemManager->ServerGrabStashedItemToCursor(AtItemPosition);
			return;
		}

		const AObsidianHUD* ObsidianHUD = OwnerPlayerController->GetObsidianHUD();
		if (ObsidianHUD && ObsidianHUD->IsInventoryOpened()) //TODO(intrxx) For now I support only Inventory <-> Stash
		{
			ItemManager->ServerTransferItemToInventory(AtItemPosition);
		}
		return;
	}
	
	
	const UObsidianInventoryItemInstance* InstanceToAddTo = PlayerStashComp->GetItemInstanceFromTabAtPosition(
		AtItemPosition);
	if (InstanceToAddTo == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Item Instance at pressed Location is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}
		
	const FDraggedItem DraggedItem = ItemManager->GetDraggedItem();
	if (const UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance) // We carry item instance.
	{
		if (DraggedInstance->IsStackable() && UObsidianItemsFunctionLibrary::IsTheSameItem(
			DraggedInstance, InstanceToAddTo))
		{
			ItemManager->ServerAddStacksFromDraggedItemToStashedItemAtSlot(AtItemPosition);
		}
		else if (PlayerStashComp->CanReplaceItemAtPosition(AtItemPosition, DraggedInstance))
		{
			ItemManager->ServerReplaceItemAtStashPosition(AtItemPosition);
		}
	}
	else if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef) // We carry item def
	{
		const UObsidianInventoryItemDefinition* DefaultObject = DraggedItemDef.GetDefaultObject();
		if (DefaultObject && DefaultObject->IsStackable() && UObsidianItemsFunctionLibrary::IsTheSameItem_WithDef(
			InstanceToAddTo, DraggedItemDef))
		{
			ItemManager->ServerAddStacksFromDraggedItemToStashedItemAtSlot(AtItemPosition);
		}
		else if(PlayerStashComp->CanReplaceItemAtPosition(AtItemPosition, DraggedItemDef))
		{
			ItemManager->ServerReplaceItemAtStashPosition(AtItemPosition);
		}
	}
}

void UObInventoryItemsWidgetController::HandleLeftClickingOnStashedItemWithShiftDown(
	const FObsidianItemPosition& AtItemPosition, const UObsidianItem* ItemWidget)
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
		CraftingComp->UseItem(AtItemPosition, true);
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
		ItemManager->ServerAddStacksFromDraggedItemToStashedItemAtSlot(AtItemPosition, 1);
		return;
	}
	
	UObsidianInventoryItemInstance* ItemInstance = PlayerStashComp->GetItemInstanceFromTabAtPosition(AtItemPosition);
	if(ItemInstance->IsStackable() == false)
	{
		return;
	}
	
	const int32 CurrentItemStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(CurrentItemStacks <= 1)
	{
		return;
	}
	
	RemoveItemUIElements(EObsidianPanelOwner::PlayerStash);

	checkf(UnstackSliderClass, TEXT("Tried to create widget without valid widget class in fill it in"
								 " UObInventoryItemsWidgetController instance."));
	ActiveUnstackSlider = CreateWidget<UObsidianUnstackSlider>(OwnerPlayerController.Get(), UnstackSliderClass);
	ActiveUnstackSlider->InitializeUnstackSlider(CurrentItemStacks, AtItemPosition);

	const FVector2D UnstackSliderViewportPosition = CalculateUnstackSliderPosition(ItemWidget);
	ActiveUnstackSlider->SetPositionInViewport(UnstackSliderViewportPosition);
	ActiveUnstackSlider->AddToViewport();
	bUnstackSliderActive = true;
	
	ActiveUnstackSlider->OnAcceptButtonPressedDelegate.AddUObject(this, &ThisClass::HandleTakingOutStacksFromStash);
	ActiveUnstackSlider->OnCloseButtonPressedDelegate.AddUObject(this, &ThisClass::RemoveUnstackSlider);
}

void UObInventoryItemsWidgetController::HandleRightClickingOnInventoryItem(const FIntPoint& AtGridSlot,
	UObsidianItem* ItemWidget)
{
	UObsidianCraftingComponent* CraftingComp = OwnerCraftingComponent.Get();
	if (CraftingComp == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Trying to trigger crafting without valid Crafting Comp"
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
	
	UObsidianInventoryItemInstance* UsingInstance = InventoryComp->GetItemInstanceAtLocation(AtGridSlot);
	if(UsingInstance && UsingInstance->IsItemUsable() == false)
	{
		return;
	}

	if(UsingInstance->GetUsableItemType() == EObsidianUsableItemType::UIT_Crafting)
	{
		//ItemWidget = ItemWidget == nullptr ? GetItemWidgetFromInventoryAtGridPosition(AtGridSlot) : ItemWidget;
		if (ItemWidget == nullptr)
		{
			UE_LOG(LogWidgetController_Items, Error, TEXT("ItemWidget is invalid in [%hs]."), __FUNCTION__);
			return;
		}
		
		CraftingComp->SetUsingItem(true, ItemWidget, UsingInstance);

		TArray<UObsidianInventoryItemInstance*> AllItems;
		AllItems.Append(InventoryComp->GetAllItems());
		AllItems.Append(EquipmentComp->GetAllEquippedItems());
		AllItems.Append(PlayerStashComp->GetAllItems()); //TODO(intrxx) This will be hella slow, change later 
	
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
		CraftingComp->ServerActivateUsableItemFromInventory(UsingInstance);
	}
}

void UObInventoryItemsWidgetController::HandleRightClickingOnStashedItem(const FObsidianItemPosition& AtItemPosition,
	UObsidianItem* ItemWidget)
{
	UObsidianCraftingComponent* CraftingComp = OwnerCraftingComponent.Get();
	if (CraftingComp == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Trying to trigger crafting without valid Crafting Comp"
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
		AtItemPosition);
	if(UsingInstance && UsingInstance->IsItemUsable() == false)
	{
		return;
	}

	if(UsingInstance->GetUsableItemType() == EObsidianUsableItemType::UIT_Crafting)
	{
		//ItemWidget = ItemWidget == nullptr ? GetItemWidgetAtStashPosition(AtItemPosition) : ItemWidget;
		if (ItemWidget == nullptr)
		{
			UE_LOG(LogWidgetController_Items, Error, TEXT("ItemWidget is invalid in [%hs]."), __FUNCTION__);
			return;
		}
		
		CraftingComp->SetUsingItem(true, ItemWidget, UsingInstance);

		TArray<UObsidianInventoryItemInstance*> AllItems;
		AllItems.Append(InventoryComp->GetAllItems());
		AllItems.Append(EquipmentComp->GetAllEquippedItems());
		AllItems.Append(PlayerStashComp->GetAllItems()); //TODO(intrxx) This will be hella slow, change later 
	
		const FObsidianItemsMatchingUsableContext MatchingUsableContext = UsingInstance->FireItemUseUIContext(AllItems);
		for(const FIntPoint& GridLocation : MatchingUsableContext.InventoryItemsMatchingContext)
		{
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
		CraftingComp->ServerActivateUsableItemFromStash(UsingInstance);
	}
}

void UObInventoryItemsWidgetController::OnStartDraggingItem(const FDraggedItem& DraggedItem)
{
	if (OwnerPlayerController == nullptr || DraggedItem.IsEmpty())
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
		if (const UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance)
		{
			for (const FObsidianEquipmentSlotDefinition& EquipmentSlot : EquipmentComp->FindPossibleSlotsForEquipping_WithInstance(
				DraggedInstance))
			{
				JoinedSlotTags.AddTag(EquipmentSlot.GetEquipmentSlotTag());
			}
		}
		else if (const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef = DraggedItem.ItemDef)
		{
			for (const FObsidianEquipmentSlotDefinition& EquipmentSlot : EquipmentComp->FindPossibleSlotsForEquipping_WithItemDef(
				ItemDef, DraggedItem.GeneratedData))
			{
				JoinedSlotTags.AddTag(EquipmentSlot.GetEquipmentSlotTag());
			}
		}
	}

	UObsidianPlayerStashComponent* StashComp = OwnerPlayerStashComponent.Get();
	if (StashComp && ObsidianHUD->IsPlayerStashOpened()) // Gather possible functional slots
	{
		if (const UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance)
		{
			for (const FObsidianStashSlotDefinition& StashSlot : StashComp->FindPossibleSlotsForPlacingItem_WithInstance(
				DraggedInstance))
			{
				JoinedSlotTags.AddTag(StashSlot.GetStashSlotTag());
			}
		}
		else if (const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef = DraggedItem.ItemDef)
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

void UObInventoryItemsWidgetController::HandleTakingOutStacksFromInventory(const int32 StacksToTake,
	const FObsidianItemPosition& ItemPosition)
{
	RemoveUnstackSlider();

	check(OwnerItemManagerComponent.IsValid());
	UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get();
	if (ItemManager == nullptr)
	{
		return;
	}
	
	if(StacksToTake == 0 || OwnerInventoryComponent == nullptr)
	{
		return;
	}

	const FIntPoint GridPosition = ItemPosition.GetItemGridPosition();
	
	if(const UObsidianInventoryItemInstance* Instance = OwnerInventoryComponent->GetItemInstanceAtLocation(
		GridPosition))
	{
		if(Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current) == StacksToTake)
		{
			ItemManager->ServerGrabInventoryItemToCursor(GridPosition);
			return;
		}
		ItemManager->ServerTakeoutFromInventoryItem(GridPosition, StacksToTake);
	}
}

void UObInventoryItemsWidgetController::HandleTakingOutStacksFromStash(const int32 StacksToTake,
	const FObsidianItemPosition& ItemPosition)
{
	RemoveUnstackSlider();

	check(OwnerItemManagerComponent.IsValid());
	UObsidianItemManagerComponent* ItemManager = OwnerItemManagerComponent.Get();
	if (ItemManager == nullptr)
	{
		return;
	}
	
	if(StacksToTake == 0 || OwnerPlayerStashComponent == nullptr)
	{
		return;
	}
	
	if(const UObsidianInventoryItemInstance* Instance = OwnerPlayerStashComponent->GetItemInstanceFromTabAtPosition(
		ItemPosition))
	{
		if(Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current) == StacksToTake)
		{
			ItemManager->ServerGrabStashedItemToCursor(ItemPosition);
			return;
		}
		ItemManager->ServerTakeoutFromStashedItem(ItemPosition, StacksToTake);
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

void UObInventoryItemsWidgetController::ClearItemDescriptionForPosition(const FObsidianItemPosition& ForPosition)
{
	FObsidianActiveItemDescriptionData DescriptionData;
	if (ActiveItemDescriptions.RemoveAndCopyValue(ForPosition, DescriptionData))
	{
		if (UObsidianItemDescriptionBase* DescriptionWidget = DescriptionData.OwningItemDescription)
		{
			DescriptionWidget->DestroyDescriptionWidget();
		}
	}
}

void UObInventoryItemsWidgetController::ClearItemDescriptionsForOwner(const EObsidianPanelOwner ForDescriptionOwner)
{
	for(auto It = ActiveItemDescriptions.CreateIterator(); It; ++It)
	{
		auto& Entry = *It;
		if(Entry.Value.DescriptionPanelOwner == ForDescriptionOwner)
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
	const FObsidianItemPosition& AtPosition, const EObsidianPanelOwner PanelOwner, const UObsidianItem* ForItemWidget,
	const FObsidianItemStats& ItemStats)
{
	ClearItemDescriptionForPosition(AtPosition); //TODO(intrxx) will it be necessary?
	
	checkf(ItemDescriptionClass, TEXT("Tried to create widget without valid widget class, fill it in "
								   "UObInventoryItemsWidgetController instance."));
	UObsidianItemDescriptionBase* NewItemDescription = CreateWidget<UObsidianItemDescriptionBase>(
		OwnerPlayerController.Get(), ItemDescriptionClass);
	NewItemDescription->InitializeWidgetWithItemStats(ItemStats);
	NewItemDescription->AddToViewport();
	
	const FVector2D DescriptionViewportPosition = CalculateDescriptionPosition(ForItemWidget, NewItemDescription);
	NewItemDescription->SetPositionInViewport(DescriptionViewportPosition);
	
	ActiveItemDescriptions.Add(AtPosition, FObsidianActiveItemDescriptionData(NewItemDescription, PanelOwner));
	
	return NewItemDescription;
}

UObsidianItemDescriptionBase* UObInventoryItemsWidgetController::CreateDroppedItemDescription(
	const FObsidianItemStats& ItemStats)
{
	check(OwnerPlayerController.IsValid());
	const AObsidianPlayerController* ObsidianPC = OwnerPlayerController.Get();
	if (ObsidianPC == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("ObsidianPC is invalid in [%hs]."), __FUNCTION__);
		return nullptr;
	}
	
	AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD();
	if(ObsidianHUD == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Unable to get ObsidianHUD in [%hs]."), __FUNCTION__);
		return nullptr;
	}

	const UObsidianMainOverlay* MainOverlay = ObsidianHUD->GetMainOverlay();
	if(MainOverlay == nullptr)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Unable to get ObsidianMainOverlay in [%hs]."),
			__FUNCTION__);
		return nullptr;
	}

	RemoveCurrentDroppedItemDescription(); // Clear any other Item Description

	checkf(ItemDescriptionClass, TEXT("Tried to create widget without valid widget class,"
								   " fill it in UObInventoryItemsWidgetController instance."));
	ActiveDroppedItemDescription = CreateWidget<UObsidianItemDescriptionBase>(OwnerPlayerController.Get(),
		ItemDescriptionClass);
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
	USlateBlueprintLibrary::LocalToViewport(World, CachedGeometry, FVector2D(0.f,0.f),
		ItemPixelPosition, ItemViewportPosition);

	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(World);
	return GetItemUIElementPositionBoundByViewport(ViewportSize, ItemPixelPosition, ItemLocalSize,
		SliderSize);
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
	USlateBlueprintLibrary::LocalToViewport(World, CachedGeometry, FVector2D(0.f,0.f),
		ItemPixelPosition, ItemViewportPosition);
	
	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(World);
	return GetItemUIElementPositionBoundByViewport(ViewportSize, ItemPixelPosition, ItemLocalSize,
		DescriptionSize);
}

FVector2D UObInventoryItemsWidgetController::GetItemUIElementPositionBoundByViewport(const FVector2D& ViewportSize,
	const FVector2D& ItemPosition, const FVector2D& ItemSize, const FVector2D& UIElementSize) const
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
		BoundedPosition = FVector2D(
			(ItemPosition.X - (UIElementSize.X / 2)) + (ItemSize.X / 2),
			(ItemPosition.Y + ItemSize.Y));
		if((BoundedPosition.Y + UIElementSize.Y) < ViewportSize.Y) // Desc fit below [bottom-middle]
		{
			return BoundedPosition;
		}
	}
	
	if(bFitsRight == false)
	{
		BoundedPosition = FVector2D(
			(ItemPosition.X - UIElementSize.X),
			(ItemPosition.Y - (UIElementSize.Y / 2)) + (ItemSize.Y / 2));
		if(BoundedPosition.X > 0.0f) // Desc fit left [left-middle]
		{
			return BoundedPosition;
		}
	}
	
	if(bFitsLeft == false)
	{
		BoundedPosition = FVector2D(
			(ItemPosition.X + ItemSize.X),
			(ItemPosition.Y - (UIElementSize.Y / 2)) + (ItemSize.Y / 2));
		if((BoundedPosition.X + UIElementSize.X) < ViewportSize.X) // Desc Fit right [right-middle]
		{
			return BoundedPosition;
		}
	}
	
	// Falling back to the default not so happy position which is most likely to fit if all above cases fail,
	// could improve it later to fit the screen in every case but is not necessary now [middle-middle].
	BoundedPosition = FVector2D(
		(ItemPosition.X - (UIElementSize.X / 2)) + (ItemSize.X / 2),
		(ItemPosition.Y - (UIElementSize.Y / 2)) + (ItemSize.Y / 2));
	
	return BoundedPosition;
}



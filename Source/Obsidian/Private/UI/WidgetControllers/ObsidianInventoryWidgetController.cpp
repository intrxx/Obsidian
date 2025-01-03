// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "CharacterComponents/ObsidianHeroComponent.h"
#include "InventoryItems/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/Inventory/ObsidianDraggedItem.h"
#include "UI/Inventory/ObsidianItem.h"

void UObsidianInventoryWidgetController::OnWidgetControllerSetupCompleted()
{
	check(InventoryComponent);
	InternalInventoryComponent = InventoryComponent;
	InventoryComponent->OnItemAddedToInventoryDelegate.AddUObject(this, &ThisClass::OnItemAdded);
	InventoryStateMap = InventoryComponent->Internal_GetInventoryStateMap();

	const AActor* OwningActor = Cast<AActor>(PlayerController->GetPawn());
	check(OwningActor);
	
	InternalHeroComponent = UObsidianHeroComponent::FindHeroComponent(OwningActor);
	check(InternalHeroComponent);
}

void UObsidianInventoryWidgetController::OnItemAdded(UObsidianInventoryItemInstance* ItemInstance, const FVector2D DesiredPosition)
{
	check(ItemInstance);
	//bInventoryChanged = true;
	InventoryStateMap = InventoryComponent->Internal_GetInventoryStateMap();
	
	const int32 StackCount = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	const FObsidianItemVisuals ItemVisuals = FObsidianItemVisuals
	(
		ItemInstance->GetItemImage(),
		DesiredPosition,
		ItemInstance->GetItemGridSpan(),
		StackCount
	);
	OnItemAddedDelegate.Broadcast(ItemVisuals);
}

void UObsidianInventoryWidgetController::OnInventoryOpen()
{
	//TODO This really needs profiling, for now let it be this way
	/*
	if(bInventoryChanged == false)
	{
		for(const TTuple<FVector2D, UObsidianInventoryItemInstance*>& LocToInstancePair : GridLocationToItemMap)
		{
			OnItemAddedDelegate.Broadcast(LocToInstancePair.Value->GetItemImage(), LocToInstancePair.Key, LocToInstancePair.Value->GetItemGridSpan());
		}
		return;
	}
	*/
	
	GridLocationToItemMap.Empty();
	GridLocationToItemMap = InventoryComponent->Internal_GetLocationToInstanceMap();
	AddedItemWidgetMap.Empty(GridLocationToItemMap.Num());
	
	for(const TTuple<FVector2D, UObsidianInventoryItemInstance*>& LocToInstancePair : GridLocationToItemMap)
	{
		const UObsidianInventoryItemInstance* ItemInstance = LocToInstancePair.Value;
		ensure(ItemInstance);
		
		const int32 StackCount = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		const FObsidianItemVisuals ItemVisuals = FObsidianItemVisuals
		(
			ItemInstance->GetItemImage(),
			LocToInstancePair.Key,
			ItemInstance->GetItemGridSpan(),
			StackCount
		);
		OnItemAddedDelegate.Broadcast(ItemVisuals);
	}

	//bInventoryChanged = false;
}

void UObsidianInventoryWidgetController::RequestAddingItemToInventory(const FVector2D& SlotPosition)
{
	if(InternalHeroComponent->IsDraggingAnItem() == false)
	{
		return;
	}
	check(InventoryComponent);
	
	const UObsidianDraggedItem* DraggedItem = InternalHeroComponent->GetCurrentlyDraggedItem();
	if(UObsidianInventoryItemInstance* Instance = DraggedItem->GetItemInstance())
	{
		if(InventoryComponent->AddItemInstanceToSpecificSlot(Instance, SlotPosition))
		{
			InternalHeroComponent->StopDragging();
		}
	}
	else
	{
		const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem->GetItemDef();
		const int32 ItemStackCount = DraggedItem->GetItemStacks();

		UObsidianInventoryItemInstance* NewInstance = InventoryComponent->AddItemDefinitionToSpecifiedSlot(ItemDef, SlotPosition, ItemStackCount);
		if(NewInstance != nullptr)
		{
			InternalHeroComponent->StopDragging();
		}
	}
}

void UObsidianInventoryWidgetController::RequestPickingUpItemFromInventory(const FVector2D& SlotPosition)
{
	check(InventoryComponent);
	check(DraggedItemWidgetClass);
	
	if(InternalHeroComponent->IsDraggingAnItem() == true) //TODO This should be a valid case in the future, just replace the item
	{
		//Replace the item if it fits
		const UObsidianDraggedItem* CachedDraggedItem = InternalHeroComponent->GetCurrentlyDraggedItem();
		if(UObsidianInventoryItemInstance* CachedDraggedInstance = CachedDraggedItem->GetItemInstance())
		{
			if(InventoryComponent->CanReplaceItemAtSpecificSlotWithInstance(SlotPosition, CachedDraggedInstance))
			{
				InternalHeroComponent->StopDragging();
				PickupItem(SlotPosition);
				InventoryComponent->AddItemInstanceToSpecificSlot(CachedDraggedInstance, SlotPosition);
			}
			return;
		}
		if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = CachedDraggedItem->GetItemDef())
		{
			InternalHeroComponent->StopDragging();
			PickupItem(SlotPosition);
			
			const int32 ItemStackCount = CachedDraggedItem->GetItemStacks();
			InventoryComponent->AddItemDefinitionToSpecifiedSlot(ItemDef, SlotPosition, ItemStackCount);
			
			return;
		}
		return;
	}
	PickupItem(SlotPosition);
}

void UObsidianInventoryWidgetController::PickupItem(const FVector2D& SlotPosition)
{
	UObsidianInventoryItemInstance* ItemInstance = InventoryComponent->Internal_GetItemInstanceAtLocation(SlotPosition);
	RemoveItemWidget(SlotPosition);
	
	UObsidianDraggedItem* DraggedItem = CreateWidget<UObsidianDraggedItem>(PlayerController, DraggedItemWidgetClass);
	DraggedItem->InitializeItemWidgetWithItemInstance(ItemInstance);
	DraggedItem->AddToViewport();
	InternalHeroComponent->DragItem(DraggedItem);

	InventoryComponent->RemoveItemInstance(ItemInstance);
	InventoryStateMap = InventoryComponent->Internal_GetInventoryStateMap();
}

bool UObsidianInventoryWidgetController::IsDraggingAnItem() const
{
	if(InternalHeroComponent)
	{
		return InternalHeroComponent->IsDraggingAnItem();
	}
	return false;
}

bool UObsidianInventoryWidgetController::CanPlaceDraggedItem(const FVector2D& HoveredSlot, const TArray<FVector2D>& ItemGridSize) const
{
	if(ItemGridSize.IsEmpty())
	{
		TArray<FVector2D> LocalItemGridSize;
		if(!GetDraggedItemGridSize(LocalItemGridSize))
		{
			return false;
		}
		return CanAddToSpecificSlot(LocalItemGridSize, HoveredSlot);
	}
	return CanAddToSpecificSlot(ItemGridSize, HoveredSlot);
}

bool UObsidianInventoryWidgetController::CanAddToSpecificSlot(const TArray<FVector2D>& ItemGridSize, const FVector2D& HoveredSlot) const
{
	bool bCanFit = false;
	
	if(InventoryStateMap[HoveredSlot] == false) // Initial location is free
	{
		bCanFit = true;
		for(FVector2D LocationComp : ItemGridSize)
		{
			const FVector2D Loc = HoveredSlot + LocationComp;
			if(!InventoryStateMap.Contains(Loc) || InventoryStateMap[Loc] == true)
			{
				bCanFit = false;
				break;
			}
		}
	}
	return bCanFit;
}

bool UObsidianInventoryWidgetController::GetDraggedItemGridSize(TArray<FVector2D>& OutItemGridSize) const
{
	if(!IsDraggingAnItem())
	{
		return false;
	}

	const UObsidianDraggedItem* DraggedItem = InternalHeroComponent->GetCurrentlyDraggedItem();
	if(!DraggedItem)
	{
		return false;
	}
	
	if(const UObsidianInventoryItemInstance* Instance = DraggedItem->GetItemInstance())
	{
		OutItemGridSize = Instance->GetItemGridSize();
		return true;	
	}
	
	if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem->GetItemDef())
	{
		if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
		{
			if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
			{
				OutItemGridSize = AppearanceFrag->GetItemGridSizeFromDesc();
				return true;	
			}
		}
	}
	return false;
}

void UObsidianInventoryWidgetController::AddItemWidget(const FVector2D& Location, UObsidianItem* ItemWidget)
{
	if(!AddedItemWidgetMap.Contains(Location))
	{
		AddedItemWidgetMap.Add(Location, ItemWidget);
	}
}

void UObsidianInventoryWidgetController::RemoveItemWidget(const FVector2D& Location)
{
	if(AddedItemWidgetMap.Contains(Location))
	{
		AddedItemWidgetMap[Location]->RemoveFromParent();
		
		AddedItemWidgetMap.Remove(Location);
	}
}

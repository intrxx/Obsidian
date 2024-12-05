// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"

#include "CharacterComponents/ObsidianHeroComponent.h"
#include "InventoryItems/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "UI/Inventory/ObsidianDraggedItem.h"
#include "UI/Inventory/ObsidianItem.h"

void UObsidianInventoryWidgetController::OnWidgetControllerSetupCompleted()
{
	check(InventoryComponent);
	InternalInventoryComponent = InventoryComponent;
	InventoryComponent->OnItemAddedToInventoryDelegate.AddUObject(this, &ThisClass::OnItemAdded);

	const AActor* OwningActor = Cast<AActor>(PlayerController->GetPawn());
	check(OwningActor);
	
	InternalHeroComponent = UObsidianHeroComponent::FindHeroComponent(OwningActor);
	check(InternalHeroComponent);
}

void UObsidianInventoryWidgetController::OnItemAdded(UObsidianInventoryItemInstance* ItemInstance, const FVector2D DesiredPosition)
{
	check(ItemInstance);
	
	OnItemAutomaticallyAddedDelegate.Broadcast(ItemInstance->GetItemImage(), DesiredPosition, ItemInstance->GetItemGridSpan());
}

void UObsidianInventoryWidgetController::OnInventoryOpen()
{
	//TODO This really needs profiling, for now let it be this way
	TMap<FVector2D, UObsidianInventoryItemInstance*> GridLocationToItemMap = InventoryComponent->Internal_GetLocationToInstanceMap();
	for(const TTuple<FVector2D, UObsidianInventoryItemInstance*>& LocToInstancePair : GridLocationToItemMap)
	{
		OnItemAutomaticallyAddedDelegate.Broadcast(LocToInstancePair.Value->GetItemImage(), LocToInstancePair.Key,
			LocToInstancePair.Value->GetItemGridSpan());
	}
}

void UObsidianInventoryWidgetController::RequestAddingItemDefToInventory(const FVector2D& SlotPosition)
{
	if(InternalHeroComponent->IsDraggingAnItem() == false)
	{
		return;
	}
	check(InventoryComponent);
	
	const UObsidianDraggedItem* DraggedItem = InternalHeroComponent->GetCurrentlyDraggedItem();
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem->GetItemDef();
	const int32 ItemStackCount = DraggedItem->GetItemStacks();

	UObsidianInventoryItemInstance* Instance = InventoryComponent->AddItemDefinitionToSpecifiedSlot(ItemDef, SlotPosition, ItemStackCount);
	if(Instance != nullptr)
	{
		InternalHeroComponent->StopDragging();
	}
}

void UObsidianInventoryWidgetController::RequestPickingUpItemFromInventory(const FVector2D& SlotPosition)
{
	if(InternalHeroComponent->IsDraggingAnItem() == true) //TODO This should be a valid case in the future, just replace the item
	{
		return;
	}
	check(InventoryComponent);

	UObsidianInventoryItemInstance* ItemInstance = InventoryComponent->Internal_GetItemInstanceForLocation(SlotPosition);
	RemoveItemWidget(SlotPosition);
	
	UObsidianDraggedItem* DraggedItem = CreateWidget<UObsidianDraggedItem>(PlayerController, DraggedItemWidgetClass);
	DraggedItem->InitializeItemWidgetWithItemInstance(ItemInstance);
	DraggedItem->AddToViewport();
	InternalHeroComponent->DragItem(DraggedItem);

	InventoryComponent->RemoveItemInstance(ItemInstance);
}

void UObsidianInventoryWidgetController::AddItemWidget(const FVector2D& Location, UObsidianItem* ItemWidget)
{
	AddedItemWidgetMap.Add(Location, ItemWidget);
}

void UObsidianInventoryWidgetController::RemoveItemWidget(const FVector2D& Location)
{
	if(AddedItemWidgetMap.Contains(Location))
	{
		AddedItemWidgetMap[Location]->RemoveFromParent();
		
		AddedItemWidgetMap.Remove(Location);
	}
}

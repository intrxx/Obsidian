// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"

#include "CharacterComponents/ObsidianHeroComponent.h"
#include "InventoryItems/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "UI/Inventory/ObsidianDraggedItem.h"

void UObsidianInventoryWidgetController::OnWidgetControllerSetupCompleted()
{
	check(InventoryComponent);
	InternalInventoryComponent = InventoryComponent;
	InventoryComponent->OnItemAddedToInventoryDelegate.AddUObject(this, &ThisClass::OnItemAdded);

	const AActor* OwningActor = Cast<AActor>(PlayerController->GetPawn());
	check(OwningActor);
	
	InternalHeroComponent = UObsidianHeroComponent::FindHeroComponent(OwningActor);
	check(InternalHeroComponent);

	// When creating the widget controller for the first time, we need to fill it with data from the component.
	GridLocationToItemMap = InventoryComponent->Internal_GetLocationToInstanceMap();
}

void UObsidianInventoryWidgetController::OnItemAdded(UObsidianInventoryItemInstance* ItemInstance, const FVector2D DesiredPosition)
{
	check(ItemInstance);
	
	OnItemAutomaticallyAddedDelegate.Broadcast(ItemInstance->GetItemImage(), DesiredPosition, ItemInstance->GetItemGridSpan());
	
	GridLocationToItemMap.Add(DesiredPosition, ItemInstance);
}

void UObsidianInventoryWidgetController::OnInventoryOpen()
{
	//TODO This really needs profiling, for now let it be this way
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

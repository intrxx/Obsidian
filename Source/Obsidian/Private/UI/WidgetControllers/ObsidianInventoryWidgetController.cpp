// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"

void UObsidianInventoryWidgetController::OnWidgetControllerSetupCompleted()
{
	check(InventoryComponent);

	InventoryComponent->OnItemAddedToInventoryDelegate.AddUObject(this, &ThisClass::OnItemAdded);
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

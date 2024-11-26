// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"

void UObsidianInventoryWidgetController::OnWidgetControllerSetupCompleted()
{
	check(InventoryComponent);

	InventoryComponent->OnItemAddedToInventoryDelegate.AddUObject(this, &ThisClass::OnItemAdded);
}

void UObsidianInventoryWidgetController::OnItemAdded(UObsidianInventoryItemInstance* ItemInstance, FVector2D DesiredPosition)
{
	check(ItemInstance);

	//TODO Fix later, pass the widget onto the instance itself,
	//maybe not use a widget but texture for the image, construct some regular widget and sets its desired size based on GridSpan
	const UOInventoryItemFragment_Appearance* Appearance = Cast<UOInventoryItemFragment_Appearance>(
		ItemInstance->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass()));

	OnItemAutomaticallyAddedDelegate.Broadcast(Appearance->ItemWidgetClass, DesiredPosition, ItemInstance->GetItemGridSpan());
}

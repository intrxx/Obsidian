// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianInventory.h"

#include "Components/Overlay.h"
#include "UI/Inventory/ObsidianItemWidget.h"
#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"

void UObsidianInventory::NativeConstruct()
{
	Super::NativeConstruct();
}

void UObsidianInventory::HandleWidgetControllerSet()
{
	InventoryWidgetController = Cast<UObsidianInventoryWidgetController>(WidgetController);
	check(InventoryWidgetController);

	InventoryWidgetController->OnItemAutomaticallyAddedDelegate.AddUObject(this, &ThisClass::OnItemAutomaticallyAdded);
}

void UObsidianInventory::OnItemAutomaticallyAdded(TSubclassOf<UObsidianItemWidget> ItemWidgetClass)
{
	check(ItemWidgetClass);
	UObsidianItemWidget* ItemWidget = CreateWidget<UObsidianItemWidget>(this, ItemWidgetClass);

	TempOverlay->AddChild(ItemWidget);
}

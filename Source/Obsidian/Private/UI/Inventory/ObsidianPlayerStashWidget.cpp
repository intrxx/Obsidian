// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianPlayerStashWidget.h"

#include "UI/WidgetControllers/ObsidianInventoryItemsWidgetController.h"

// ~ Core

// ~ Project
#include "UI/WidgetControllers/ObsidianInventoryItemsWidgetController.h"

void UObsidianPlayerStashWidget::HandleWidgetControllerSet()
{
	Super::HandleWidgetControllerSet();

	InventoryItemsWidgetController = Cast<UObsidianInventoryItemsWidgetController>(WidgetController);
	check(InventoryItemsWidgetController);
}

void UObsidianPlayerStashWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
}

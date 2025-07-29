// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianPlayerStashWidget.h"

#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"

// ~ Core

// ~ Project
#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"

void UObsidianPlayerStashWidget::HandleWidgetControllerSet()
{
	Super::HandleWidgetControllerSet();

	InventoryWidgetController = Cast<UObsidianInventoryWidgetController>(WidgetController);
	check(InventoryWidgetController);
}

void UObsidianPlayerStashWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
}

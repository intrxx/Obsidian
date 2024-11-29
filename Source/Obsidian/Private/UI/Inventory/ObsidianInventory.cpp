// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianInventory.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
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

void UObsidianInventory::OnItemAutomaticallyAdded(UTexture2D* ItemImage, const FVector2D DesiredPosition, const FVector2D GridSpan)
{
	UObsidianItemWidget* ItemWidget = CreateWidget<UObsidianItemWidget>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(GridSpan, ItemImage);

	UGridSlot* GridSlot = Slots_GridPanel->AddChildToGrid(ItemWidget, DesiredPosition.Y, DesiredPosition.X);
	GridSlot->SetLayer(1);
	GridSlot->SetColumnSpan(GridSpan.X);
	GridSlot->SetRowSpan(GridSpan.Y);
}

// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianInventory.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "UI/Inventory/SubWidgets/ObsidianInventorySlot.h"
#include "UI/Inventory/ObsidianItemWidget.h"
#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"

UObsidianInventory::UObsidianInventory()
	: InventoryGridSize(InventoryGridWidth * InventoryGridHeight)
{
}

void UObsidianInventory::NativeConstruct()
{
	Super::NativeConstruct();

	SetupGrid();
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
	ItemWidget->InitializeItemWidget(DesiredPosition, GridSpan, ItemImage);
	ItemWidget->OnItemLeftMouseButtonPressedDelegate.AddUObject(this, &ThisClass::OnItemLeftMouseButtonPressed);

	UGridSlot* GridSlot = Slots_GridPanel->AddChildToGrid(ItemWidget, DesiredPosition.Y, DesiredPosition.X);
	GridSlot->SetLayer(1);
	GridSlot->SetColumnSpan(GridSpan.X);
	GridSlot->SetRowSpan(GridSpan.Y);
}

void UObsidianInventory::SetupGrid()
{
	Slots_GridPanel->ClearChildren(); 
	
	int16 GridX = 0;
	int16 GridY = 0;
	
	for(int32 i = 0; i < InventoryGridSize; i++)
	{
		UObsidianInventorySlot* InventorySlot = CreateWidget<UObsidianInventorySlot>(this, InventorySlotClass);
		InventorySlot->OnHoverOverSlotDelegate.AddUObject(this, &ThisClass::OnInventorySlotHoverOver);
		
		UGridSlot* GridSlot = Slots_GridPanel->AddChildToGrid(InventorySlot, GridY, GridX);
		GridSlot->SetLayer(0);
		
		if(GridX == InventoryGridWidth - 1)
		{
			GridX = 0;
			GridY++;
		}
		else
		{
			GridX++;
		}
	}
}

void UObsidianInventory::OnItemLeftMouseButtonPressed(const FVector2D ItemDesiredPosition)
{
	UE_LOG(LogTemp, Warning, TEXT("Hi Mouse, Item here."));
}

void UObsidianInventory::OnInventorySlotHoverOver()
{
	
}
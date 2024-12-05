// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianInventory.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "UI/Inventory/SubWidgets/ObsidianInventorySlot.h"
#include "UI/Inventory/ObsidianItem.h"
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

	InventoryWidgetController->OnItemAddedDelegate.AddUObject(this, &ThisClass::OnItemAdded);
}

void UObsidianInventory::OnItemAdded(UTexture2D* ItemImage, const FVector2D DesiredPosition, const FVector2D GridSpan)
{
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(DesiredPosition, GridSpan, ItemImage);
	ItemWidget->OnItemLeftMouseButtonPressedDelegate.AddUObject(this, &ThisClass::OnItemLeftMouseButtonPressed);
	
	InventoryWidgetController->AddItemWidget(DesiredPosition, ItemWidget);

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
		InventorySlot->SetSlotPosition(FVector2D(GridX, GridY));
		InventorySlot->OnHoverOverSlotDelegate.AddUObject(this, &ThisClass::OnInventorySlotHoverOver);
		InventorySlot->OnMouseButtonDownOnSlotDelegate.AddUObject(this, &ThisClass::OnInventorySlotMouseButtonDown);
		
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
	if(InventoryWidgetController)
	{
		InventoryWidgetController->RequestPickingUpItemFromInventory(ItemDesiredPosition);
	}
}

void UObsidianInventory::OnInventorySlotHoverOver(bool bEntered)
{
	
}

void UObsidianInventory::OnInventorySlotMouseButtonDown(const FVector2D& SlotPosition)
{
	if(InventoryWidgetController)
	{
		InventoryWidgetController->RequestAddingItemToInventory(SlotPosition);
	}
}

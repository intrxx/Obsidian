// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianInventory.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/SizeBox.h"
#include "UI/Inventory/SubWidgets/ObsidianInventorySlot.h"
#include "UI/Inventory/ObsidianItem.h"
#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"

UObsidianInventory::UObsidianInventory()
	: InventoryGridSize(InventoryGridWidth * InventoryGridHeight)
{
}

void UObsidianInventory::NativePreConstruct()
{
	Super::NativePreConstruct();

	Root_SizeBox->SetHeightOverride(RootSizeBoxHeight);
	Root_SizeBox->SetWidthOverride(RootSizeBoxWidth);
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

void UObsidianInventory::SetupGrid() 
{
	if(Slots_GridPanel->HasAnyChildren())
	{
		Slots_GridPanel->ClearChildren(); 
	}
	
	int16 GridX = 0;
	int16 GridY = 0;
	
	for(int32 i = 0; i < InventoryGridSize; i++)
	{
		const FVector2D SlotPosition = FVector2D(GridX, GridY);
		UObsidianInventorySlot* InventorySlot = CreateWidget<UObsidianInventorySlot>(this, InventorySlotClass);
		InventorySlot->SetSlotPosition(SlotPosition);
		InventorySlot->OnHoverOverSlotDelegate.AddUObject(this, &ThisClass::OnInventorySlotHover);
		InventorySlot->OnMouseButtonDownOnSlotDelegate.AddUObject(this, &ThisClass::OnInventorySlotMouseButtonDown);
		
		UGridSlot* GridSlot = Slots_GridPanel->AddChildToGrid(InventorySlot, GridY, GridX);
		GridSlot->SetLayer(0);

		InventoryLocationToSlotMap.Add(SlotPosition, InventorySlot);
		
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

void UObsidianInventory::OnItemAdded(const FObsidianItemVisuals& ItemVisuals)
{
	const FVector2D DesiredPosition = ItemVisuals.DesiredPosition;
	const FVector2D GridSpan = ItemVisuals.GridSpan;
	
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(DesiredPosition, GridSpan, ItemVisuals.ItemImage, ItemVisuals.StackCount);
	ItemWidget->OnItemLeftMouseButtonPressedDelegate.AddUObject(this, &ThisClass::OnItemLeftMouseButtonPressed);
	ItemWidget->OnItemMouseEnterDelegate.AddUObject(this, &ThisClass::OnItemMouseEntered);
	ItemWidget->OnItemMouseLeaveDelegate.AddUObject(this, &ThisClass::OnItemMouseLeave);
	InventoryWidgetController->AddItemWidget(DesiredPosition, ItemWidget);

	UGridSlot* GridSlot = Slots_GridPanel->AddChildToGrid(ItemWidget, DesiredPosition.Y, DesiredPosition.X);
	GridSlot->SetLayer(1);
	GridSlot->SetColumnSpan(GridSpan.X);
	GridSlot->SetRowSpan(GridSpan.Y);
}

void UObsidianInventory::OnItemLeftMouseButtonPressed(const FVector2D& ItemDesiredPosition, UObsidianItem* ItemWidget, const bool bShiftDown)
{
	if(bShiftDown)
	{
		InventoryWidgetController->HandleLeftClickingOnAnItemWithShiftDown(ItemDesiredPosition, ItemWidget);
		return;
	}
	InventoryWidgetController->HandleLeftClickingOnAnItem(ItemDesiredPosition, ItemWidget);
}

void UObsidianInventory::OnItemMouseEntered(const FVector2D& ItemDesiredPosition, UObsidianItem* ItemWidget)
{
	InventoryWidgetController->HandleHoveringOverItem(ItemDesiredPosition, ItemWidget);
}

void UObsidianInventory::OnItemMouseLeave(const FVector2D& ItemDesiredPosition)
{
	InventoryWidgetController->HandleUnhoveringItem(ItemDesiredPosition);
}

void UObsidianInventory::OnInventorySlotHover(const bool bEntered, UObsidianInventorySlot* AffectedSlot)
{
	if(bEntered)
	{
		if(!InventoryWidgetController || !InventoryWidgetController->IsDraggingAnItem())
		{
			return;
		}

		TArray<FVector2D> ItemGridSize;
		const bool bSuccess = InventoryWidgetController->GetDraggedItemGridSize(ItemGridSize);
		if(!bSuccess)
		{
			return;
		}
		
		const FVector2D HoveredSlotPosition = AffectedSlot->GetSlotPosition();
		const bool bCanPlace = InventoryWidgetController->CanPlaceDraggedItem(HoveredSlotPosition, ItemGridSize);
		
		for(FVector2D SizeComp : ItemGridSize)
		{
			const FVector2D LocationToCheck = HoveredSlotPosition + SizeComp;
			if(InventoryLocationToSlotMap.Contains(LocationToCheck))
			{
				UObsidianInventorySlot* LocalSlot = InventoryLocationToSlotMap[LocationToCheck];
				LocalSlot->SetSlotState(bCanPlace);
				AffectedSlots.Add(LocalSlot);
			}
		}
	}
	else
	{
		if(AffectedSlots.IsEmpty())
		{
			return;
		}

		for(UObsidianInventorySlot* InventorySlot : AffectedSlots)
		{
			InventorySlot->ResetSlot();
		}
		AffectedSlots.Empty();
	}
}

void UObsidianInventory::OnInventorySlotMouseButtonDown(const FVector2D& SlotPosition, const bool bShiftDown)
{
	if(InventoryWidgetController)
	{
		InventoryWidgetController->RequestAddingItemToInventory(SlotPosition, bShiftDown);
	}
}

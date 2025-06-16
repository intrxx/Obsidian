// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianInventoryGrid.h"

// ~ Core
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

// ~ Project
#include "UI/Inventory/SubWidgets/ObsidianItemSlot_Inventory.h"
#include "UI/Inventory/ObsidianInventory.h"
#include "UI/Inventory/ObsidianItem.h"

void UObsidianInventoryGrid::ConstructInventoryGrid(UObsidianInventory* InOwningInventory, const int32 InventoryGridWidth, const int32 InventoryGridHeight)
{
	OwningInventory = InOwningInventory;
	
	if(Root_CanvasPanel->HasAnyChildren())
	{
		Root_CanvasPanel->ClearChildren();
	}
	
	const int32 InventoryGridSize = InventoryGridHeight * InventoryGridWidth;
	GridSlots.Reserve(InventoryGridSize);
	
	int16 GridX = 0;
	int16 GridY = 0;
	for(int32 i = 0; i < InventoryGridSize; i++)
	{
		const FIntPoint SlotPosition = FIntPoint(GridX, GridY);
		
		checkf(InventorySlotClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::SetupGrid, fill it in ObsidianInventory instance."));
		UObsidianItemSlot_Inventory* InventorySlot = CreateWidget<UObsidianItemSlot_Inventory>(this, InventorySlotClass);
		InventorySlot->InitializeSlot(this, SlotPosition);

		UCanvasPanelSlot* AddedSlot = Root_CanvasPanel->AddChildToCanvas(InventorySlot);
		AddedSlot->SetSize(FVector2D(SlotTileSize));
		AddedSlot->SetPosition(SlotPosition * SlotTileSize);
		
		GridSlots.Add(InventorySlot);
		
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

UObsidianItemSlot_Inventory* UObsidianInventoryGrid::GetSlotByPosition(const FIntPoint& BySlotPosition)
{
	const int32 Index = Algo::BinarySearchBy(GridSlots, BySlotPosition,[](const UObsidianItemSlot_Inventory* PotentialSlot)
		{
			return PotentialSlot->GetSlotPosition();
		},
		[](const FIntPoint& A, const FIntPoint& B)
		{
			if (A.Y < B.Y) return true;
			if (A.Y > B.Y) return false;
			return A.X < B.X;
		});

	return Index != INDEX_NONE ? GridSlots[Index] : nullptr;
}

void UObsidianInventoryGrid::OnInventorySlotHover(UObsidianItemSlot_Inventory* AffectedSlot, const bool bEntered)
{
	if(OwningInventory.IsValid() == false)
	{
		return;
	}
	
	if(bEntered)
	{
		FIntPoint ItemGridSpan = FIntPoint::NoneValue;
		if(OwningInventory->GetDraggedItemGridSpan(ItemGridSpan) == false) 
		{
			AffectedSlot->SetSlotState(ISS_Selected);
			AffectedInventorySlots.Add(AffectedSlot);
			return; 
		}
		
		const FIntPoint HoveredSlotPosition = AffectedSlot->GetSlotPosition();
		const bool bCanPlace = OwningInventory->CanPlaceDraggedItem(HoveredSlotPosition, ItemGridSpan);
		for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
		{
			for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
			{
				const FIntPoint LocationToCheck = HoveredSlotPosition + FIntPoint(SpanX, SpanY);
				if(UObsidianItemSlot_Inventory* LocalSlot = GetSlotByPosition(LocationToCheck))
				{
					const EObsidianItemSlotState SlotState = bCanPlace ? ISS_GreenLight : ISS_RedLight;
					LocalSlot->SetSlotState(SlotState);
					AffectedInventorySlots.Add(LocalSlot);
				}
			}	
		}
	}
	else
	{
		if(AffectedInventorySlots.IsEmpty())
		{
			return;
		}

		for(const UObsidianItemSlot_Inventory* InventorySlot : AffectedInventorySlots)
		{
			InventorySlot->SetSlotState(ISS_Neutral);
		}
		AffectedInventorySlots.Empty();
	}
}

void UObsidianInventoryGrid::OnInventorySlotMouseButtonDown(const UObsidianItemSlot_Inventory* AffectedSlot, const bool bShiftDown) const
{
	OnInventoryGridSlotPressedDelegate.Broadcast(AffectedSlot->GetSlotPosition(), bShiftDown);
}

void UObsidianInventoryGrid::AddItemToGrid(UObsidianItem* ItemWidget, const float ItemSlotPadding)
{
	UCanvasPanelSlot* CanvasItem = Root_CanvasPanel->AddChildToCanvas(ItemWidget);
	CanvasItem->SetSize(ItemWidget->GetItemWidgetSize());
	
	const FVector2D ItemPosition = SlotTileSize * (FVector2D)ItemWidget->GetInventoryPosition() + ItemSlotPadding;
	CanvasItem->SetPosition(ItemPosition);
}


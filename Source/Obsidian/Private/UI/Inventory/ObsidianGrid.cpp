// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianGrid.h"

// ~ Core
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

// ~ Project
#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot_GridSlot.h"
#include "UI/Inventory/Items/ObsidianItem.h"
#include "UI/WidgetControllers/ObsidianInventoryItemsWidgetController.h"

void UObsidianGrid::ConstructGrid(UObsidianInventoryItemsWidgetController* InOwningWidgetController, const EObsidianGridOwner InGridOwner, const int32 GridWidth, const int32 GridHeight, const FGameplayTag& OptionalStashTag)
{
	if(InOwningWidgetController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("InOwningWidgetController is nullptr in [%hs], please fix it."), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	GridOwner = InGridOwner;
	StashTag = OptionalStashTag;
	OwningWidgetController = InOwningWidgetController;
	
	if(Root_CanvasPanel->HasAnyChildren())
	{
		Root_CanvasPanel->ClearChildren();
	}
	
	const int32 InventoryGridSize = GridHeight * GridWidth;
	GridSlots.Reserve(InventoryGridSize);
	
	int16 GridX = 0;
	int16 GridY = 0;
	for(int32 i = 0; i < InventoryGridSize; i++)
	{
		const FIntPoint SlotPosition = FIntPoint(GridX, GridY);
		
		checkf(GridSlotClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::SetupGrid, fill it in ObsidianInventory instance."));
		UObsidianItemSlot_GridSlot* InventorySlot = CreateWidget<UObsidianItemSlot_GridSlot>(this, GridSlotClass);
		InventorySlot->InitializeSlot(this, SlotPosition);

		UCanvasPanelSlot* AddedSlot = Root_CanvasPanel->AddChildToCanvas(InventorySlot);
		AddedSlot->SetSize(FVector2D(SlotTileSize));
		AddedSlot->SetPosition(SlotPosition * SlotTileSize);
		
		GridSlots.Add(InventorySlot);
		
		if(GridX == GridWidth - 1)
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

UObsidianItemSlot_GridSlot* UObsidianGrid::GetSlotByPosition(const FIntPoint& BySlotPosition)
{
	const int32 Index = Algo::BinarySearchBy(GridSlots, BySlotPosition,[](const UObsidianItemSlot_GridSlot* PotentialSlot)
		{
			return PotentialSlot->GetSlotPositionOnGrid();
		},
		[](const FIntPoint& A, const FIntPoint& B)
		{
			if (A.Y < B.Y) return true;
			if (A.Y > B.Y) return false;
			return A.X < B.X;
		});

	return Index != INDEX_NONE ? GridSlots[Index] : nullptr;
}

void UObsidianGrid::OnInventorySlotHover(UObsidianItemSlot_GridSlot* AffectedSlot, const bool bEntered)
{
	if(OwningWidgetController.IsValid() == false)
	{
		return;
	}
	
	if(bEntered)
	{
		FIntPoint ItemGridSpan = FIntPoint::NoneValue;
		if(OwningWidgetController->GetDraggedItemGridSpan(ItemGridSpan) == false) 
		{
			//TODO This isn't quite what I want here since the grid will be used anywhere, should be dealt with other way
			const EObsidianItemSlotState SlotState = OwningWidgetController->CanInteractWithGrid(GridOwner) ? ISS_Selected : ISS_RedLight;
			AffectedSlot->SetSlotState(SlotState);
			AffectedGridSlots.Add(AffectedSlot);
			return; 
		}
		
		const FIntPoint HoveredSlotPosition = AffectedSlot->GetSlotPositionOnGrid();
		const bool bCanPlace = OwningWidgetController->CanPlaceDraggedItem(GridOwner, HoveredSlotPosition, ItemGridSpan, StashTag);
		for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
		{
			for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
			{
				const FIntPoint LocationToCheck = HoveredSlotPosition + FIntPoint(SpanX, SpanY);
				if(UObsidianItemSlot_GridSlot* LocalSlot = GetSlotByPosition(LocationToCheck))
				{
					const EObsidianItemSlotState SlotState = bCanPlace ? ISS_GreenLight : ISS_RedLight;
					LocalSlot->SetSlotState(SlotState);
					AffectedGridSlots.Add(LocalSlot);
				}
			}	
		}
	}
	else
	{
		if(AffectedGridSlots.IsEmpty())
		{
			return;
		}

		for(const UObsidianItemSlot_GridSlot* InventorySlot : AffectedGridSlots)
		{
			InventorySlot->SetSlotState(ISS_Neutral);
		}
		AffectedGridSlots.Empty();
	}
}

void UObsidianGrid::OnInventorySlotMouseButtonDown(const UObsidianItemSlot_GridSlot* AffectedSlot, const bool bShiftDown) const
{
	OnGridSlotPressedDelegate.Broadcast(AffectedSlot->GetSlotPositionOnGrid(), bShiftDown);
}

void UObsidianGrid::AddItemToGrid(UObsidianItem* ItemWidget, const float ItemSlotPadding)
{
	UCanvasPanelSlot* CanvasItem = Root_CanvasPanel->AddChildToCanvas(ItemWidget);
	const FVector2D ItemSize = FVector2D(
		(ItemWidget->GetItemGridSpan().X * SlotTileSize) - (ItemSlotPadding * 2),
		(ItemWidget->GetItemGridSpan().Y * SlotTileSize) - (ItemSlotPadding * 2)
		);
	CanvasItem->SetSize(ItemSize);
	
	const FVector2D ItemPosition = SlotTileSize * static_cast<FVector2D>(ItemWidget->GetGridPosition()) + ItemSlotPadding;
	CanvasItem->SetPosition(ItemPosition);
}


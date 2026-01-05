// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianGrid.h"

#include <Components/CanvasPanel.h>
#include <Components/CanvasPanelSlot.h>

#include "Obsidian/ObsidianGameModule.h"
#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot_GridSlot.h"
#include "UI/Inventory/Items/ObsidianItem.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"

// ~ Start of FObsidianOccupiedPlacement

void FObsidianOccupiedPlacement::Reset()
{
	ItemWidget = nullptr;
	OriginPosition = FIntPoint::NoneValue;
	ItemGridSpan = FIntPoint::NoneValue;
	bOccupied = false;
}

// ~ End of FObsidianOccupiedPlacement

void UObsidianGrid::HandleWidgetControllerSet()
{
	InventoryItemsWidgetController = Cast<UObInventoryItemsWidgetController>(WidgetController);
	check(InventoryItemsWidgetController);
	
}

void UObsidianGrid::ConstructGrid(const EObsidianGridOwner InGridOwner, const int32 GridWidth, const int32 GridHeight,
	const FGameplayTag& OptionalStashTag)
{
	checkf(GridSlotClass, TEXT("Tried to create widget without valid widget class in [%hs],"
							 " fill it in ObsidianInventory instance."), __FUNCTION__);
	
	GridOwner = InGridOwner;
	StashTag = OptionalStashTag;
	
	if(Root_CanvasPanel->HasAnyChildren())
	{
		Root_CanvasPanel->ClearChildren();
	}
	
	const int32 InventoryGridSize = GridHeight * GridWidth;
	GridSlotsMap.Empty(InventoryGridSize);
	
	int16 GridX = 0;
	int16 GridY = 0;
	for(int32 i = 0; i < InventoryGridSize; i++)
	{
		const FIntPoint SlotPosition = FIntPoint(GridX, GridY);
		
		UObsidianItemSlot_GridSlot* InventorySlot = CreateWidget<UObsidianItemSlot_GridSlot>(this, GridSlotClass);
		InventorySlot->InitializeSlot(SlotPosition);
		InventorySlot->OnGridSlotHoverDelegate.AddUObject(this, &ThisClass::OnInventorySlotHover);
		InventorySlot->OnGridSlotLeftButtonPressedDelegate.AddUObject(this, &ThisClass::OnInventorySlotLeftMouseButtonDown);
		InventorySlot->OnGridSlotRightButtonPressedDelegate.AddUObject(this, &ThisClass::OnInventorySlotRightMouseButtonDown);

		UCanvasPanelSlot* AddedSlot = Root_CanvasPanel->AddChildToCanvas(InventorySlot);
		AddedSlot->SetSize(FVector2D(SlotTileSize));
		AddedSlot->SetPosition(SlotPosition * SlotTileSize);
		
		GridSlotsMap.Add(SlotPosition, InventorySlot);
		InventorizedItemWidgetMap.Add(SlotPosition, FObsidianOccupiedPlacement());
		
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

void UObsidianGrid::NativeDestruct()
{
	for (const TPair<FIntPoint, UObsidianItemSlot_GridSlot*>& GridSlotPair : GridSlotsMap)
	{
		if (GridSlotPair.Value)
		{
			GridSlotPair.Value->OnGridSlotHoverDelegate.Clear();
			GridSlotPair.Value->OnGridSlotLeftButtonPressedDelegate.Clear();
			GridSlotPair.Value->OnGridSlotRightButtonPressedDelegate.Clear();
		}
	}
	
	Super::NativeDestruct();
}

UObsidianItemSlot_GridSlot* UObsidianGrid::GetSlotByPosition(const FIntPoint& BySlotPosition)
{
	if (UObsidianItemSlot_GridSlot** GridSlotValuePtr = GridSlotsMap.Find(BySlotPosition))
	{
		return *GridSlotValuePtr;
	}
	return nullptr;
}

FObsidianOccupiedPlacement* UObsidianGrid::GetSlotPlacementAtGridPosition(const FIntPoint& AtGridPosition)
{
	return InventorizedItemWidgetMap.Find(AtGridPosition);
}

UObsidianItem* UObsidianGrid::GetItemWidgetAtGridPosition(const FIntPoint& AtGridPosition)
{
	if(const FObsidianOccupiedPlacement* OccupiedPlacement = InventorizedItemWidgetMap.Find(AtGridPosition))
	{
		return OccupiedPlacement->ItemWidget;
	}
	return nullptr;
}

bool UObsidianGrid::IsGridSlotOccupied(const FIntPoint& AtGridPosition) const
{
	if(const FObsidianOccupiedPlacement* OccupiedPlacement = InventorizedItemWidgetMap.Find(AtGridPosition))
	{
		return OccupiedPlacement->bOccupied;
	}
	return false;
}

void UObsidianGrid::AddItemWidget(UObsidianItem* ItemWidget, const FObsidianItemWidgetData& ItemWidgetData)
{
	if (ItemWidget == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("ItemWidget to Add Item Widget to Grid is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}
	
	const FIntPoint ItemGridPosition = ItemWidgetData.ItemPosition.GetItemGridPosition();
	if (ItemGridPosition == FIntPoint::NoneValue)
	{
		UE_LOG(LogObsidian, Error, TEXT("Desired Grid Slot Position to Add Item Widget to Grid is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}

	const float ItemSlotPadding = ItemWidgetData.ItemSlotPadding;
	UCanvasPanelSlot* CanvasItem = Root_CanvasPanel->AddChildToCanvas(ItemWidget);
	const FVector2D ItemSize = FVector2D(
		(ItemWidget->GetItemGridSpan().X * SlotTileSize) - (ItemSlotPadding * 2),
		(ItemWidget->GetItemGridSpan().Y * SlotTileSize) - (ItemSlotPadding * 2)
		);
	CanvasItem->SetSize(ItemSize);
	
	const FVector2D ItemPosition = SlotTileSize * static_cast<FVector2D>(ItemGridPosition) + ItemSlotPadding;
	CanvasItem->SetPosition(ItemPosition);

	RegisterInventoryItemWidget(ItemGridPosition, ItemWidget);
}

void UObsidianGrid::OnInventorySlotHover(UObsidianItemSlot_GridSlot* AffectedSlot, const bool bEntered)
{
	if(InventoryItemsWidgetController == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("InventoryItemsWidgetController is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	if (AffectedSlot == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("AffectedSlot is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	FIntPoint GridSlotPosition = AffectedSlot->GetGridSlotPosition();
	check(GridSlotPosition != FIntPoint::NoneValue);

	const bool bCanInteractWithGrid = InventoryItemsWidgetController->CanInteractWithGrid(GridOwner);
	const bool bIsDraggingAnItem = InventoryItemsWidgetController->IsDraggingAnItem();
	const FObsidianOccupiedPlacement* Placement = GetSlotPlacementAtGridPosition(GridSlotPosition);
	const bool bIsSlotOccupied = Placement && Placement->bOccupied;
	
	if(bEntered)
	{
		if (bIsSlotOccupied)
		{
			InventoryItemsWidgetController->HandleHoveringOverItem(Placement->OriginPosition,
				Placement->ItemWidget);
		}

		if (bIsDraggingAnItem)
		{
			FIntPoint ItemGridSpan;
			InventoryItemsWidgetController->GetDraggedItemGridSpan(ItemGridSpan);
			
			OffsetGridPositionByItemSpan(ItemGridSpan, GridSlotPosition);

			bool bCanPlace = false;
			if (bCanInteractWithGrid)
			{
				//TODO(intrxx) Override for the actual clicked position
				bCanPlace = InventoryItemsWidgetController->CanPlaceDraggedItem(GridOwner, GridSlotPosition, ItemGridSpan,
					StashTag);
			}
			
			for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
			{
				for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
				{
					const FIntPoint LocationToCheck = GridSlotPosition + FIntPoint(SpanX, SpanY);
					if(UObsidianItemSlot_GridSlot* LocalSlot = GetSlotByPosition(LocationToCheck))
					{
						const EObsidianItemSlotState SlotState = bCanPlace ? EObsidianItemSlotState::GreenLight :
							EObsidianItemSlotState::RedLight;
						LocalSlot->SetSlotState(SlotState, EObsidianItemSlotStatePriority::Low);
						AffectedGridSlots.Add(LocalSlot);
					}
				}	
			}
			
			return;
		}

		if (bCanInteractWithGrid == false)
		{
			AffectedSlot->SetSlotState(EObsidianItemSlotState::RedLight, EObsidianItemSlotStatePriority::Low);
			AffectedGridSlots.Add(AffectedSlot);
			return;
		}

		AffectedSlot->SetSlotState(EObsidianItemSlotState::Selected, EObsidianItemSlotStatePriority::Low);
		AffectedGridSlots.Add(AffectedSlot);
	}
	else
	{
		if (bIsSlotOccupied)
		{
			//TODO(intrxx) this will not work for stash
			InventoryItemsWidgetController->HandleUnhoveringItem(Placement->OriginPosition);
		}
		
		if(AffectedGridSlots.IsEmpty() == false)
		{
			for(UObsidianItemSlot_GridSlot* InventorySlot : AffectedGridSlots)
			{
				InventorySlot->SetSlotState(EObsidianItemSlotState::Neutral, EObsidianItemSlotStatePriority::Low);
			}
			AffectedGridSlots.Empty();
		}
	}
}

void UObsidianGrid::OnInventorySlotLeftMouseButtonDown(const UObsidianItemSlot_GridSlot* AffectedSlot,
	const FObsidianItemInteractionFlags& InteractionFlags)
{
	if(InventoryItemsWidgetController == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("InventoryItemsWidgetController is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	if (AffectedSlot == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("AffectedSlot is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	FIntPoint OriginGridPositionPressed = AffectedSlot->GetGridSlotPosition();
	check(OriginGridPositionPressed != FIntPoint::NoneValue);
	
	FIntPoint OutDraggedItemGridSpan;
	if (InventoryItemsWidgetController && InventoryItemsWidgetController->GetDraggedItemGridSpan(OutDraggedItemGridSpan))
	{
		// It feels kinda forced :/ Probably will need to rethink the whole thing someday
		OffsetGridPositionByItemSpan(OutDraggedItemGridSpan, OriginGridPositionPressed);
	}

	const FObsidianOccupiedPlacement* Placement = GetSlotPlacementAtGridPosition(OriginGridPositionPressed);
	if (Placement && Placement->bOccupied)
	{
		if(InteractionFlags.bItemStacksInteraction)
		{
			InventoryItemsWidgetController->HandleLeftClickingOnInventoryItemWithShiftDown(Placement->OriginPosition,
				Placement->ItemWidget);
		}
		else
		{
			//TODO(intrxx) Override for the actual clicked position
			InventoryItemsWidgetController->HandleLeftClickingOnInventoryItem(Placement->OriginPosition,
				InteractionFlags.bMoveBetweenNextOpenedWindow);	
		}
	}
	else
	{
		//TODO(intrxx) Override for the actual clicked position
		InventoryItemsWidgetController->RequestAddingItemToInventory(OriginGridPositionPressed,
			InteractionFlags.bItemStacksInteraction);
	}
}

void UObsidianGrid::OnInventorySlotRightMouseButtonDown(const UObsidianItemSlot_GridSlot* AffectedSlot,
	const FObsidianItemInteractionFlags& InteractionFlags)
{
	if(InventoryItemsWidgetController == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("InventoryItemsWidgetController is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	if (AffectedSlot == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("AffectedSlot is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	const FIntPoint GridSlotPosition = AffectedSlot->GetGridSlotPosition();
	check(GridSlotPosition != FIntPoint::NoneValue);

	const FObsidianOccupiedPlacement* Placement = GetSlotPlacementAtGridPosition(GridSlotPosition);
	if (Placement && Placement->bOccupied)
	{
		InventoryItemsWidgetController->HandleRightClickingOnInventoryItem(Placement->OriginPosition, Placement->ItemWidget);
	}
}

void UObsidianGrid::RegisterInventoryItemWidget(const FIntPoint& GridSlot, UObsidianItem* ItemWidget)
{
	if (ensureMsgf(ItemWidget && GridSlot != FIntPoint::NoneValue, TEXT("ItemWidget or GridSlot are invalid in [%hs]."),
		__FUNCTION__))
	{
		FObsidianOccupiedPlacement OccupiedPlacement;
		OccupiedPlacement.ItemWidget = ItemWidget;
		OccupiedPlacement.OriginPosition = GridSlot;
		OccupiedPlacement.ItemGridSpan = ItemWidget->GetItemGridSpan();
		OccupiedPlacement.bOccupied = true;
			
		for(int32 SpanX = 0; SpanX < OccupiedPlacement.ItemGridSpan.X; ++SpanX)
		{
			for(int32 SpanY = 0; SpanY < OccupiedPlacement.ItemGridSpan.Y; ++SpanY)
			{
				const FIntPoint LocationToOccupy = GridSlot + FIntPoint(SpanX, SpanY);
				if(FObsidianOccupiedPlacement* Placement = InventorizedItemWidgetMap.Find(LocationToOccupy))
				{
					ensure(Placement->bOccupied == false);
					*Placement = OccupiedPlacement;
				}
			}	
		}
	}
}

void UObsidianGrid::HandleItemRemoved(const FIntPoint& GridSlot)
{
	if (ensureMsgf(GridSlot != FIntPoint::NoneValue, TEXT("GridSlot is invalid in [%hs]."), __FUNCTION__))
	{
		if (const FObsidianOccupiedPlacement* FoundPlacement = InventorizedItemWidgetMap.Find(GridSlot))
		{
			check(FoundPlacement->bOccupied);
			
			if (FoundPlacement->ItemWidget == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("Trying to remove ItemWidget from [%s], but the ItemWidget is invalid!"),
					*GridSlot.ToString());
				return;
			}

			FoundPlacement->ItemWidget->RemoveFromParent();

			const FIntPoint OccupiedGridSpan = FoundPlacement->ItemGridSpan;
			for(int32 SpanX = 0; SpanX < OccupiedGridSpan.X; ++SpanX)
			{
				for(int32 SpanY = 0; SpanY < OccupiedGridSpan.Y; ++SpanY)
				{
					const FIntPoint LocationToReset = GridSlot + FIntPoint(SpanX, SpanY);
					if(FObsidianOccupiedPlacement* Placement = InventorizedItemWidgetMap.Find(LocationToReset))
					{
						Placement->Reset();
					}
				}	
			}
			
			if (ensure(InventoryItemsWidgetController))
			{
				//TODO(intrxx) this will not work for Stash Tabs
				InventoryItemsWidgetController->ClearItemDescriptionForPosition(GridSlot);
			}
		}
	}
}

void UObsidianGrid::HandleItemStackChanged(const FIntPoint& AtGridSlot, const uint8 NewStackCount)
{
	if(UObsidianItem* ItemWidget = GetItemWidgetAtGridPosition(AtGridSlot))
	{
		ItemWidget->OverrideCurrentStackCount(NewStackCount);
	}
}

void UObsidianGrid::OffsetGridPositionByItemSpan(FIntPoint DraggedItemGridSpan, FIntPoint& OriginalPosition) const 
{
	DraggedItemGridSpan = FIntPoint(
			(DraggedItemGridSpan.X % 2 == 0) ? (DraggedItemGridSpan.X - 1) / 2 : DraggedItemGridSpan.X / 2,
			(DraggedItemGridSpan.Y % 2 == 0) ? (DraggedItemGridSpan.Y - 1) / 2 : DraggedItemGridSpan.Y / 2);
	
	OriginalPosition -= DraggedItemGridSpan;
}


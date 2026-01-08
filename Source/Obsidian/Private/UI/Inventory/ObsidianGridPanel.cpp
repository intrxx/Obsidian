// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianGridPanel.h"

#include <Components/CanvasPanel.h>
#include <Components/CanvasPanelSlot.h>

#include "Obsidian/ObsidianGameModule.h"
#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot_GridSlot.h"
#include "UI/Inventory/Items/ObsidianItem.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"

// ~ Start of FObsidianGridSlotData

bool FObsidianGridSlotData::IsOccupied() const
{
	return bOccupied;
}

void FObsidianGridSlotData::AddNewItem(const FObsidianItemPosition& InPosition, UObsidianItem* InItemWidget,
	const FIntPoint InItemGridSpan)
{
	OriginPosition = InPosition;
	ItemWidget = InItemWidget;
	ItemGridSpan = InItemGridSpan;
	bOccupied = true;
}

void FObsidianGridSlotData::Reset()
{
	OriginPosition.Reset();
	ItemWidget = nullptr;
	ItemGridSpan = FIntPoint::NoneValue;
	bOccupied = false;
}

// ~ End of FObsidianGridSlotData

void UObsidianGridPanel::HandleWidgetControllerSet()
{
	InventoryItemsWidgetController = Cast<UObInventoryItemsWidgetController>(WidgetController);
	check(InventoryItemsWidgetController);
}

void UObsidianGridPanel::NativeDestruct()
{
	for (const TPair<FIntPoint, FObsidianGridSlotData>& GridSlotPair : GridSlotDataMap)
	{
		if (UObsidianItemSlot_GridSlot* GridSlot = GridSlotPair.Value.OwningGridSlot)
		{
			GridSlot->OnGridSlotHoverDelegate.Clear();
			GridSlot->OnGridSlotLeftButtonPressedDelegate.Clear();
			GridSlot->OnGridSlotRightButtonPressedDelegate.Clear();
		}
	}
	
	Super::NativeDestruct();
}

bool UObsidianGridPanel::ConstructInventoryPanel()
{
	if (InventoryItemsWidgetController == nullptr)
	{
		return false;
	}
	
	PanelOwner = EObsidianPanelOwner::Inventory;
	return ConstructGrid(InventoryItemsWidgetController->GetInventoryGridWidth(),
				InventoryItemsWidgetController->GetInventoryGridHeight());
}

bool UObsidianGridPanel::ConstructStashPanel(const int32 GridWidthOverride, const int32 GridHeightOverride,
	const FGameplayTag& InStashTag)
{
	if (InStashTag.IsValid() == false)
	{
		return false;
	}
	
	StashTag = InStashTag;
	PanelOwner = EObsidianPanelOwner::PlayerStash;
	return ConstructGrid(GridWidthOverride, GridHeightOverride);
}

bool UObsidianGridPanel::ConstructGrid(const int32 GridWidth, const int32 GridHeight)
{
	checkf(GridSlotClass, TEXT("Tried to create widget without valid widget class in [%hs],"
							 " fill it in ObsidianInventory instance."), __FUNCTION__);
	
	if(Root_CanvasPanel->HasAnyChildren())
	{
		Root_CanvasPanel->ClearChildren();
	}
	
	const int32 GridSize = GridWidth * GridHeight;
	GridSlotDataMap.Empty(GridSize);
	
	int16 GridX = 0;
	int16 GridY = 0;
	for(int32 i = 0; i < GridSize; i++)
	{
		const FIntPoint SlotPosition = FIntPoint(GridX, GridY);
		
		UObsidianItemSlot_GridSlot* GridSlot = CreateWidget<UObsidianItemSlot_GridSlot>(this, GridSlotClass);
		GridSlot->InitializeSlot(SlotPosition);
		GridSlot->OnGridSlotHoverDelegate.AddUObject(this, &ThisClass::OnGridSlotHover);
		GridSlot->OnGridSlotLeftButtonPressedDelegate.AddUObject(this, &ThisClass::OnGridSlotLeftMouseButtonDown);
		GridSlot->OnGridSlotRightButtonPressedDelegate.AddUObject(this, &ThisClass::OnGridSlotRightMouseButtonDown);

		UCanvasPanelSlot* AddedSlot = Root_CanvasPanel->AddChildToCanvas(GridSlot);
		AddedSlot->SetSize(FVector2D(SlotTileSize));
		AddedSlot->SetPosition(SlotPosition * SlotTileSize);
		
		FObsidianGridSlotData NewData;
		NewData.OwningGridSlot = GridSlot;
		GridSlotDataMap.Add(SlotPosition, NewData);
		
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

	if (GridSize == GridSlotDataMap.Num())
	{
		return true;
	}
	return false;
}

UObsidianItemSlot_GridSlot* UObsidianGridPanel::GetSlotByPosition(const FIntPoint& BySlotPosition)
{
	if (const FObsidianGridSlotData* GridSlotData = GridSlotDataMap.Find(BySlotPosition))
	{
		return GridSlotData->OwningGridSlot;
	}
	return nullptr;
}

const FObsidianGridSlotData* UObsidianGridPanel::GetSlotDataAtGridPosition(const FIntPoint& AtGridPosition) const
{
	return GridSlotDataMap.Find(AtGridPosition);
}

UObsidianItem* UObsidianGridPanel::GetItemWidgetAtGridPosition(const FIntPoint& AtGridPosition) const
{
	const FObsidianGridSlotData* GridSlotData = GridSlotDataMap.Find(AtGridPosition);
	if(GridSlotData && GridSlotData->IsOccupied())
	{
		return GridSlotData->ItemWidget;
	}
	return nullptr;
}

bool UObsidianGridPanel::IsGridSlotOccupied(const FIntPoint& AtGridPosition) const
{
	if(const FObsidianGridSlotData* GridSlotData = GridSlotDataMap.Find(AtGridPosition))
	{
		return GridSlotData->IsOccupied();
	}
	return false;
}

void UObsidianGridPanel::AddItemWidget(UObsidianItem* ItemWidget, const FObsidianItemWidgetData& ItemWidgetData)
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

	const FIntPoint ItemGridSpan = ItemWidgetData.GridSpan;
	const float ItemSlotPadding = ItemWidgetData.ItemSlotPadding;
	UCanvasPanelSlot* CanvasItem = Root_CanvasPanel->AddChildToCanvas(ItemWidget);
	const FVector2D ItemSize = FVector2D(
		(ItemGridSpan.X * SlotTileSize) - (ItemSlotPadding * 2),
		(ItemGridSpan.Y * SlotTileSize) - (ItemSlotPadding * 2)
		);
	CanvasItem->SetSize(ItemSize);
	
	const FVector2D ItemPosition = SlotTileSize * static_cast<FVector2D>(ItemGridPosition) + ItemSlotPadding;
	CanvasItem->SetPosition(ItemPosition);

	RegisterGridItemWidget(ItemWidgetData.ItemPosition, ItemWidget, ItemGridSpan);
}

void UObsidianGridPanel::OnGridSlotHover(UObsidianItemSlot_GridSlot* AffectedSlot, const bool bEntered)
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

	const bool bCanInteractWithGrid = InventoryItemsWidgetController->CanInteractWithGrid(PanelOwner);
	const bool bIsDraggingAnItem = InventoryItemsWidgetController->IsDraggingAnItem();
	const FObsidianGridSlotData* SlotData = GetSlotDataAtGridPosition(GridSlotPosition);
	const bool bIsSlotOccupied = SlotData && SlotData->IsOccupied();
	const FIntPoint OriginGridSlotPosition = bIsSlotOccupied ? SlotData->OriginPosition.GetItemGridPosition() :
		FIntPoint::NoneValue;
	
	if(bEntered)
	{
		if (bIsSlotOccupied)
		{
			InventoryItemsWidgetController->HandleHoveringOverItem(SlotData->OriginPosition,
				SlotData->ItemWidget);
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
				bCanPlace = InventoryItemsWidgetController->CanPlaceDraggedItem(PanelOwner, GridSlotPosition, ItemGridSpan,
					StashTag);
			}

			const EObsidianItemSlotState SlotState = bCanPlace ? EObsidianItemSlotState::GreenLight :
				EObsidianItemSlotState::RedLight;
			SetSlotStateForGridSlots(GridSlotPosition, ItemGridSpan, SlotState);
			return;
		}

		if (bCanInteractWithGrid == false)
		{
			AffectedSlot->SetSlotState(EObsidianItemSlotState::RedLight, EObsidianItemSlotStatePriority::Low);
			AffectedGridSlots.Add(AffectedSlot);
			return;
		}

		if (bIsSlotOccupied)
		{
			SetSlotStateForGridSlots(OriginGridSlotPosition, SlotData->ItemGridSpan, EObsidianItemSlotState::Selected);
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
			InventoryItemsWidgetController->HandleUnhoveringItem(SlotData->OriginPosition);
		}
		
		ResetGridSlotsState();
	}
}

void UObsidianGridPanel::OnGridSlotLeftMouseButtonDown(const UObsidianItemSlot_GridSlot* AffectedSlot,
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

	const FObsidianGridSlotData* SlotData = GetSlotDataAtGridPosition(OriginGridPositionPressed);
	if (SlotData && SlotData->IsOccupied())
	{
		if(InteractionFlags.bItemStacksInteraction)
		{
			InventoryItemsWidgetController->HandleLeftClickingOnInventoryItemWithShiftDown(SlotData->OriginPosition.GetItemGridPosition(),
				SlotData->ItemWidget);
		}
		else
		{
			//TODO(intrxx) Override for the actual clicked position
			InventoryItemsWidgetController->HandleLeftClickingOnInventoryItem(SlotData->OriginPosition.GetItemGridPosition(),
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

void UObsidianGridPanel::OnGridSlotRightMouseButtonDown(const UObsidianItemSlot_GridSlot* AffectedSlot,
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

	const FObsidianGridSlotData* SlotData = GetSlotDataAtGridPosition(GridSlotPosition);
	if (SlotData && SlotData->IsOccupied())
	{
		InventoryItemsWidgetController->HandleRightClickingOnInventoryItem(SlotData->OriginPosition.GetItemGridPosition(),
			SlotData->ItemWidget);
	}
}

void UObsidianGridPanel::RegisterGridItemWidget(const FObsidianItemPosition& ItemPosition, UObsidianItem* ItemWidget,
	const FIntPoint GridSpan)
{
	if (ensureMsgf(ItemWidget && ItemPosition.IsValid(), TEXT("ItemWidget or ItemPosition are invalid in [%hs]."),
		__FUNCTION__))
	{
		const FIntPoint GridSlotPosition = ItemPosition.GetItemGridPosition();
		for(int32 SpanX = 0; SpanX < GridSpan.X; ++SpanX)
		{
			for(int32 SpanY = 0; SpanY < GridSpan.Y; ++SpanY)
			{
				const FIntPoint LocationToOccupy = GridSlotPosition + FIntPoint(SpanX, SpanY);
				if(FObsidianGridSlotData* SlotData = GridSlotDataMap.Find(LocationToOccupy))
				{
					check(SlotData->IsOccupied() == false);
					SlotData->AddNewItem(ItemPosition, ItemWidget, GridSpan);
				}
			}	
		}
	}
}

void UObsidianGridPanel::HandleItemRemoved(const FObsidianItemWidgetData& ItemWidgetData)
{
	const FIntPoint GridPositionToClear = ItemWidgetData.ItemPosition.GetItemGridPosition();
	if (ensureMsgf(GridPositionToClear != FIntPoint::NoneValue, TEXT("FromPosition is invalid in [%hs]."), __FUNCTION__))
	{
		if (const FObsidianGridSlotData* SlotData = GridSlotDataMap.Find(GridPositionToClear))
		{
			check(SlotData->IsOccupied());

			UObsidianItem* SlottedItemWidget = SlotData->ItemWidget;
			if (SlottedItemWidget == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("Trying to remove ItemWidget from [%s], but the ItemWidget is invalid!"),
					*GridPositionToClear.ToString());
				return;
			}

			SlottedItemWidget->RemoveFromParent();

			const FIntPoint OccupiedGridSpan = SlotData->ItemGridSpan;
			for(int32 SpanX = 0; SpanX < OccupiedGridSpan.X; ++SpanX)
			{
				for(int32 SpanY = 0; SpanY < OccupiedGridSpan.Y; ++SpanY)
				{
					const FIntPoint LocationToReset = GridPositionToClear + FIntPoint(SpanX, SpanY);
					if(FObsidianGridSlotData* NextSlotData = GridSlotDataMap.Find(LocationToReset))
					{
						NextSlotData->Reset();
					}
				}	
			}
		}
	}
}

void UObsidianGridPanel::HandleItemStackChanged(const FIntPoint& AtGridSlot, const uint8 NewStackCount)
{
	if(UObsidianItem* ItemWidget = GetItemWidgetAtGridPosition(AtGridSlot))
	{
		ItemWidget->OverrideCurrentStackCount(NewStackCount);
	}
}

void UObsidianGridPanel::OffsetGridPositionByItemSpan(FIntPoint DraggedItemGridSpan, FIntPoint& OriginalPosition) const 
{
	DraggedItemGridSpan = FIntPoint(
			(DraggedItemGridSpan.X % 2 == 0) ? (DraggedItemGridSpan.X - 1) / 2 : DraggedItemGridSpan.X / 2,
			(DraggedItemGridSpan.Y % 2 == 0) ? (DraggedItemGridSpan.Y - 1) / 2 : DraggedItemGridSpan.Y / 2);
	
	OriginalPosition -= DraggedItemGridSpan;
}

void UObsidianGridPanel::SetSlotStateForGridSlots(const FIntPoint OriginPosition, const FIntPoint ItemGridSpan,
	const EObsidianItemSlotState SlotState)
{
	for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
	{
		for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
		{
			const FIntPoint SlotPosition = OriginPosition + FIntPoint(SpanX, SpanY);
			if(UObsidianItemSlot_GridSlot* LocalSlot = GetSlotByPosition(SlotPosition))
			{
				LocalSlot->SetSlotState(SlotState, EObsidianItemSlotStatePriority::Low);
				AffectedGridSlots.Add(LocalSlot);
			}
		}	
	}
}

void UObsidianGridPanel::ResetGridSlotsState()
{
	if(AffectedGridSlots.IsEmpty() == false)
	{
		for(UObsidianItemSlot_GridSlot* InventorySlot : AffectedGridSlots)
		{
			InventorySlot->SetSlotState(EObsidianItemSlotState::Neutral, EObsidianItemSlotStatePriority::Low);
		}
		AffectedGridSlots.Reset();
	}
}


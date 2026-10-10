// Copyright 2026 out of sCope team - intrxx

#include "UI/InventoryItems/ObsidianGridPanel.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

#include "Obsidian/ObsidianLogCategories.h"
#include "Obsidian/Public/UI/InventoryItems/Slots/ObsidianSlot_GridSlot.h"
#include "UI/InventoryItems/Items/ObsidianItem.h"
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
		if (UObsidianSlot_GridSlot* GridSlot = GridSlotPair.Value.OwningGridSlot)
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

bool UObsidianGridPanel::ConstructStashPanel(const int32 InGridWidthOverride, const int32 InGridHeightOverride,
	const FGameplayTag& InStashTag)
{
	if (InStashTag.IsValid() == false)
	{
		return false;
	}
	
	StashTag = InStashTag;
	PanelOwner = EObsidianPanelOwner::PlayerStash;
	return ConstructGrid(InGridWidthOverride, InGridHeightOverride);
}

bool UObsidianGridPanel::ConstructGrid(const int32 InGridWidth, const int32 InGridHeight)
{
	checkf(GridSlotClass, TEXT("Tried to create widget without valid widget class in [%hs],"
							 " fill it in ObsidianInventory instance."), __FUNCTION__);
	
	if(Root_CanvasPanel->HasAnyChildren())
	{
		Root_CanvasPanel->ClearChildren();
	}
	
	const int32 GridSize = InGridWidth * InGridHeight;
	GridSlotDataMap.Empty(GridSize);
	
	int16 GridX = 0;
	int16 GridY = 0;
	for(int32 i = 0; i < GridSize; i++)
	{
		const FIntPoint SlotPosition = FIntPoint(GridX, GridY);
		
		UObsidianSlot_GridSlot* GridSlot = CreateWidget<UObsidianSlot_GridSlot>(this, GridSlotClass);
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
		
		if(GridX == InGridWidth - 1)
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

UObsidianSlot_GridSlot* UObsidianGridPanel::GetSlotByPosition(const FIntPoint& InBySlotPosition)
{
	if (const FObsidianGridSlotData* GridSlotData = GridSlotDataMap.Find(InBySlotPosition))
	{
		return GridSlotData->OwningGridSlot;
	}
	return nullptr;
}

const FObsidianGridSlotData* UObsidianGridPanel::GetSlotDataAtGridPosition(const FIntPoint& InAtGridPosition) const
{
	return GridSlotDataMap.Find(InAtGridPosition);
}

UObsidianItem* UObsidianGridPanel::GetItemWidgetAtGridPosition(const FIntPoint& InAtGridPosition) const
{
	const FObsidianGridSlotData* GridSlotData = GridSlotDataMap.Find(InAtGridPosition);
	if(GridSlotData && GridSlotData->IsOccupied())
	{
		return GridSlotData->ItemWidget;
	}
	return nullptr;
}

bool UObsidianGridPanel::IsGridSlotOccupied(const FIntPoint& InAtGridPosition) const
{
	if(const FObsidianGridSlotData* GridSlotData = GridSlotDataMap.Find(InAtGridPosition))
	{
		return GridSlotData->IsOccupied();
	}
	return false;
}

void UObsidianGridPanel::AddItemWidget(UObsidianItem* InItemWidget, const FObsidianItemWidgetData& InItemWidgetData)
{
	if (InItemWidget == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("ItemWidget to Add Item Widget to Grid is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}
	
	const FIntPoint ItemGridPosition = InItemWidgetData.ItemPosition.GetItemGridPosition();
	if (ItemGridPosition == FIntPoint::NoneValue)
	{
		UE_LOG(ObLogItems, Error, TEXT("Desired Grid Slot Position to Add Item Widget to Grid is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}

	const FIntPoint ItemGridSpan = InItemWidgetData.GridSpan;
	const float ItemSlotPadding = InItemWidgetData.ItemSlotPadding;
	UCanvasPanelSlot* CanvasItem = Root_CanvasPanel->AddChildToCanvas(InItemWidget);
	const FVector2D ItemSize = FVector2D(
		(ItemGridSpan.X * SlotTileSize) - (ItemSlotPadding * 2),
		(ItemGridSpan.Y * SlotTileSize) - (ItemSlotPadding * 2)
		);
	CanvasItem->SetSize(ItemSize);
	
	const FVector2D ItemPosition = SlotTileSize * static_cast<FVector2D>(ItemGridPosition) + ItemSlotPadding;
	CanvasItem->SetPosition(ItemPosition);

	RegisterGridItemWidget(InItemWidgetData.ItemPosition, InItemWidget, ItemGridSpan);
}

void UObsidianGridPanel::OnGridSlotHover(UObsidianSlot_GridSlot* InAffectedSlot, const bool bInEntered)
{
	if(InventoryItemsWidgetController == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("InventoryItemsWidgetController is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	if (InAffectedSlot == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("AffectedSlot is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	FIntPoint GridSlotPosition = InAffectedSlot->GetGridSlotPosition();
	check(GridSlotPosition != FIntPoint::NoneValue);

	const bool bCanInteractWithGrid = InventoryItemsWidgetController->CanInteractWithGrid(PanelOwner);
	const bool bIsDraggingAnItem = InventoryItemsWidgetController->IsDraggingAnItem();
	const FObsidianGridSlotData* SlotData = GetSlotDataAtGridPosition(GridSlotPosition);
	const bool bIsSlotOccupied = SlotData && SlotData->IsOccupied();
	const FIntPoint OriginGridSlotPosition = bIsSlotOccupied ? SlotData->OriginPosition.GetItemGridPosition() :
		FIntPoint::NoneValue;
	
	if(bInEntered)
	{
		if (bIsSlotOccupied)
		{
			FObsidianItemInteractionData InteractionData;
			InteractionData.ItemWidget = SlotData->ItemWidget;
			InventoryItemsWidgetController->HandleHoveringOverItem(SlotData->OriginPosition, InteractionData,
				PanelOwner);
		}

		if (bIsDraggingAnItem)
		{
			
			const FIntPoint ItemGridSpan = InventoryItemsWidgetController->GetDraggedItemGridSpan();
			OffsetGridPositionByItemSpan(ItemGridSpan, GridSlotPosition);

			bool bCanPlace = false;
			if (bCanInteractWithGrid)
			{
				//TODO(intrxx) Override for the actual clicked position
				FObsidianItemPosition ItemPosition;
				ConstructItemPosition(ItemPosition, GridSlotPosition);
				bCanPlace = InventoryItemsWidgetController->CanPlaceDraggedItemAtPosition(ItemPosition, PanelOwner);
			}

			const EObsidianItemSlotState SlotState = bCanPlace ? EObsidianItemSlotState::GreenLight :
				EObsidianItemSlotState::RedLight;
			SetSlotStateForGridSlots(GridSlotPosition, ItemGridSpan, SlotState);
			return;
		}

		if (bCanInteractWithGrid == false)
		{
			InAffectedSlot->SetSlotState(EObsidianItemSlotState::RedLight, EObsidianItemSlotStatePriority::Low);
			AffectedGridSlots.Add(InAffectedSlot);
			return;
		}

		if (bIsSlotOccupied)
		{
			SetSlotStateForGridSlots(OriginGridSlotPosition, SlotData->ItemGridSpan, EObsidianItemSlotState::Selected);
			return;
		}
		
		InAffectedSlot->SetSlotState(EObsidianItemSlotState::Selected, EObsidianItemSlotStatePriority::Low);
		AffectedGridSlots.Add(InAffectedSlot);
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

void UObsidianGridPanel::OnGridSlotLeftMouseButtonDown(const UObsidianSlot_GridSlot* InAffectedSlot,
	const FObsidianItemInteractionFlags& InInteractionFlags)
{
	if(InventoryItemsWidgetController == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("InventoryItemsWidgetController is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	if (InAffectedSlot == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("AffectedSlot is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	FIntPoint GridPositionPressed = InAffectedSlot->GetGridSlotPosition();
	check(GridPositionPressed != FIntPoint::NoneValue);
	
	// It feels kinda forced :/ Probably will need to rethink the whole thing someday
	OffsetGridPositionByItemSpan(InventoryItemsWidgetController->GetDraggedItemGridSpan(), GridPositionPressed);

	const FObsidianGridSlotData* SlotData = GetSlotDataAtGridPosition(GridPositionPressed);
	if (SlotData == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Could not find SlotData for [%s] in [%hs]."),
			*GridPositionPressed.ToString(), __FUNCTION__)
		return;
	}
	
	FObsidianItemPosition ItemPositionToAddTo;
	ConstructItemPosition(ItemPositionToAddTo, GridPositionPressed);

	FObsidianItemInteractionData InteractionData;
	InteractionData.InteractionFlags = InInteractionFlags;
	
	if (SlotData && SlotData->IsOccupied())
	{
		FObsidianItemPosition PressedSlot;
		ConstructItemPosition(PressedSlot, GridPositionPressed);
		InteractionData.InteractionTargetPositionOverride = PressedSlot;
		InteractionData.ItemWidget = SlotData->ItemWidget;
		InventoryItemsWidgetController->HandleLeftClickingOnItem(SlotData->OriginPosition, InteractionData, PanelOwner);
	}
	else
	{
		InventoryItemsWidgetController->HandleLeftClickingOnSlot(ItemPositionToAddTo, InteractionData, PanelOwner);
	}
}

void UObsidianGridPanel::OnGridSlotRightMouseButtonDown(const UObsidianSlot_GridSlot* InAffectedSlot,
	const FObsidianItemInteractionFlags& InInteractionFlags)
{
	if(InventoryItemsWidgetController == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("InventoryItemsWidgetController is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	if (InAffectedSlot == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("AffectedSlot is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	const FIntPoint GridPositionPressed = InAffectedSlot->GetGridSlotPosition();
	check(GridPositionPressed != FIntPoint::NoneValue);

	const FObsidianGridSlotData* SlotData = GetSlotDataAtGridPosition(GridPositionPressed);
	if (SlotData == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Could not find SlotData for [%s] in [%hs]."),
			*GridPositionPressed.ToString(), __FUNCTION__)
		return;
	}

	FObsidianItemPosition ItemPositionToAddTo;
	ConstructItemPosition(ItemPositionToAddTo, GridPositionPressed);

	FObsidianItemInteractionData InteractionData;
	InteractionData.InteractionFlags = InInteractionFlags;
	
	if (SlotData && SlotData->IsOccupied())
	{
		InteractionData.ItemWidget = SlotData->ItemWidget;
		InventoryItemsWidgetController->HandleRightClickingOnItem(SlotData->OriginPosition, InteractionData, PanelOwner);
	}
	else
	{
		InventoryItemsWidgetController->HandleRightClickingOnSlot(ItemPositionToAddTo, InteractionData, PanelOwner);
	}
}

void UObsidianGridPanel::ConstructItemPosition(FObsidianItemPosition& OutItemPosition, const FIntPoint InSlotPositionOverride) const
{
	if (PanelOwner == EObsidianPanelOwner::Inventory)
	{
		OutItemPosition = FObsidianItemPosition(InSlotPositionOverride);
	}
	else if (PanelOwner == EObsidianPanelOwner::PlayerStash)
	{
		OutItemPosition = FObsidianItemPosition(InSlotPositionOverride, StashTag);
	}
}

void UObsidianGridPanel::RegisterGridItemWidget(const FObsidianItemPosition& InItemPosition, UObsidianItem* InItemWidget,
                                                const FIntPoint InGridSpan)
{
	if (ensureMsgf(InItemWidget && InItemPosition.IsValid(), TEXT("ItemWidget or ItemPosition are invalid in [%hs]."),
		__FUNCTION__))
	{
		const FIntPoint GridSlotPosition = InItemPosition.GetItemGridPosition();
		for(int32 SpanX = 0; SpanX < InGridSpan.X; ++SpanX)
		{
			for(int32 SpanY = 0; SpanY < InGridSpan.Y; ++SpanY)
			{
				const FIntPoint LocationToOccupy = GridSlotPosition + FIntPoint(SpanX, SpanY);
				if(FObsidianGridSlotData* SlotData = GridSlotDataMap.Find(LocationToOccupy))
				{
					check(SlotData->IsOccupied() == false);
					SlotData->AddNewItem(InItemPosition, InItemWidget, InGridSpan);
				}
			}	
		}
	}
}

void UObsidianGridPanel::HandleItemRemoved(const FObsidianItemWidgetData& InItemWidgetData)
{
	const FIntPoint GridPositionToClear = InItemWidgetData.ItemPosition.GetItemGridPosition();
	if (ensureMsgf(GridPositionToClear != FIntPoint::NoneValue, TEXT("FromPosition is invalid in [%hs]."), __FUNCTION__))
	{
		if (const FObsidianGridSlotData* SlotData = GridSlotDataMap.Find(GridPositionToClear))
		{
			check(SlotData->IsOccupied());

			UObsidianItem* SlottedItemWidget = SlotData->ItemWidget;
			if (SlottedItemWidget == nullptr)
			{
				UE_LOG(ObLogItems, Error, TEXT("Trying to remove ItemWidget from [%s], but the ItemWidget is invalid!"),
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

void UObsidianGridPanel::HandleItemChanged(const FObsidianItemWidgetData& InItemWidgetData)
{
	const FIntPoint AtGridSlot = InItemWidgetData.ItemPosition.GetItemGridPosition();
	if(UObsidianItem* ItemWidget = GetItemWidgetAtGridPosition(AtGridSlot))
	{
		if (InItemWidgetData.bUpdateStacks)
		{
			ItemWidget->OverrideCurrentStackCount(InItemWidgetData.StackCount);
		}
		else if (InItemWidgetData.bGeneralItemUpdate)
		{
			if (HighlightedItems.Remove(ItemWidget))
			{
				ItemWidget->ResetHighlight();

				if (InventoryItemsWidgetController)
				{
					FObsidianItemInteractionData InteractionData;
					InteractionData.ItemWidget = ItemWidget;
					InventoryItemsWidgetController->HandleHoveringOverItem(InItemWidgetData.ItemPosition, InteractionData,
						PanelOwner);
				}
			}
		}
	}
}

void UObsidianGridPanel::HandleHighlightingItems(const TArray<FObsidianItemPosition>& InItemsToHighlight)
{
	HighlightedItems.Reserve(InItemsToHighlight.Num());
	
	for (const FObsidianItemPosition& ItemPosition : InItemsToHighlight)
	{
		if (UObsidianItem* ItemWidget = GetItemWidgetAtGridPosition(ItemPosition.GetItemGridPosition()))
		{
			ItemWidget->HighlightItem();
			HighlightedItems.Add(ItemWidget);
		}
	}
}

void UObsidianGridPanel::ClearUsableItemHighlight()
{
	for (UObsidianItem* HighlightedWidget : HighlightedItems)
	{
		if (HighlightedWidget)
		{
			HighlightedWidget->ResetHighlight();
		}
	}

	HighlightedItems.Empty();
}

void UObsidianGridPanel::OffsetGridPositionByItemSpan(FIntPoint InDraggedItemGridSpan, FIntPoint& InOutOriginalPosition) const 
{
	if (InDraggedItemGridSpan == FIntPoint::NoneValue)
	{
		return;
	}
	
	InDraggedItemGridSpan = FIntPoint(
			(InDraggedItemGridSpan.X % 2 == 0) ? (InDraggedItemGridSpan.X - 1) / 2 : InDraggedItemGridSpan.X / 2,
			(InDraggedItemGridSpan.Y % 2 == 0) ? (InDraggedItemGridSpan.Y - 1) / 2 : InDraggedItemGridSpan.Y / 2);
	
	InOutOriginalPosition -= InDraggedItemGridSpan;
}

void UObsidianGridPanel::SetSlotStateForGridSlots(const FIntPoint InOriginPosition, const FIntPoint InItemGridSpan,
	const EObsidianItemSlotState InSlotState)
{
	for(int32 SpanX = 0; SpanX < InItemGridSpan.X; ++SpanX)
	{
		for(int32 SpanY = 0; SpanY < InItemGridSpan.Y; ++SpanY)
		{
			const FIntPoint SlotPosition = InOriginPosition + FIntPoint(SpanX, SpanY);
			if(UObsidianSlot_GridSlot* LocalSlot = GetSlotByPosition(SlotPosition))
			{
				LocalSlot->SetSlotState(InSlotState, EObsidianItemSlotStatePriority::Low);
				AffectedGridSlots.Add(LocalSlot);
			}
		}	
	}
}

void UObsidianGridPanel::ResetGridSlotsState()
{
	if(AffectedGridSlots.IsEmpty() == false)
	{
		for(UObsidianSlot_GridSlot* InventorySlot : AffectedGridSlots)
		{
			InventorySlot->SetSlotState(EObsidianItemSlotState::Neutral, EObsidianItemSlotStatePriority::Low);
		}
		AffectedGridSlots.Reset();
	}
}


// Copyright 2026 out of sCope - intrxx

#include "UI/InventoryItems/ObsidianItemLabelOverlay.h"

#include <Components/CanvasPanel.h>

#include "Components/CanvasPanelSlot.h"
#include "Obsidian/ObsidianGameModule.h"
#include "UI/InventoryItems/Items/ObsidianItemLabel.h"

UCanvasPanelSlot* UObsidianItemLabelOverlay::AddItemLabelToOverlay(UObsidianItemLabel* ItemLabelWidget,
	const FVector2D& AtPosition)
{
	if (ItemLabelWidget == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("Passed ItemLabelWidget is invalid in [%hs]."), __FUNCTION__);
		return nullptr;
	}
	
	UCanvasPanelSlot* CanvasSlot = Root_CanvasPanel->AddChildToCanvas(ItemLabelWidget);
	CanvasSlot->SetAutoSize(true);
	CanvasSlot->SetAlignment(FVector2D(0.5f, 1.0f));
	CanvasSlot->SetPosition(AtPosition);

	return CanvasSlot;
}

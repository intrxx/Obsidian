// Copyright 2026 out of sCope - intrxx

#pragma once

#include <CoreMinimal.h>


#include "UI/ObsidianWidgetBase.h"
#include "ObsidianItemLabelOverlay.generated.h"

class UCanvasPanelSlot;
class UObsidianItemLabel;
class UCanvasPanel;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemLabelOverlay : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	UCanvasPanelSlot* AddItemLabelToOverlay(UObsidianItemLabel* ItemLabelWidget, const FVector2D& AtPosition);
	UCanvasPanelSlot* AddItemLabelToOverlayDebug(UUserWidget* ItemLabelWidget, const FVector2D& AtPosition);

	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> Root_CanvasPanel;
};

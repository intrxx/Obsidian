// Copyright 2026 out of sCope - intrxx

#pragma once

#include <CoreMinimal.h>


#include "UI/ObsidianWidgetBase.h"
#include "ObsidianItemLabelOverlay.generated.h"

class UCanvasPanel;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemLabelOverlay : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:

	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> Root_CanvasPanel;
};

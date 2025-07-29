// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianMainOverlayWidgetBase.h"
#include "ObsidianPlayerStashWidget.generated.h"

class UObsidianGrid;
class UObsidianInventoryWidgetController;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianPlayerStashWidget : public UObsidianMainOverlayWidgetBase
{
	GENERATED_BODY()

public:
	virtual void HandleWidgetControllerSet() override;
	
protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	TObjectPtr<UObsidianInventoryWidgetController> InventoryWidgetController;
};

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianMainOverlayWidgetBase.h"
#include "ObsidianInventory.generated.h"

class UGridPanel;
class UOverlay;
class UObsidianItemWidget;
class UObsidianInventoryWidgetController;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianInventory : public UObsidianMainOverlayWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void HandleWidgetControllerSet() override;
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UGridPanel> Slots_GridPanel;

private:
	void OnItemAutomaticallyAdded(TSubclassOf<UObsidianItemWidget> ItemWidgetClass, FVector2D DesiredPosition, FVector2D GridSpan);
	
private:
	UPROPERTY()
	TObjectPtr<UObsidianInventoryWidgetController> InventoryWidgetController;
};

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianMainOverlayWidgetBase.h"
#include "ObsidianInventory.generated.h"

class UOverlay;
class UObsidianItemWidget;
class UObsidianInventoryWidgetController;
class UUniformGridPanel;

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
	TObjectPtr<UUniformGridPanel> Slots_GridPanel;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> TempOverlay;

private:
	void OnItemAutomaticallyAdded(TSubclassOf<UObsidianItemWidget> ItemWidgetClass);
	
private:
	UPROPERTY()
	TObjectPtr<UObsidianInventoryWidgetController> InventoryWidgetController;
};

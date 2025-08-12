// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianMainOverlayWidgetBase.h"
#include "ObsidianPlayerStashWidget.generated.h"

class UOverlay;
class UScrollBox;
class UObsidianStashTabWidget;
class UObsidianGrid;
class UObsidianInventoryItemsWidgetController;

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

protected:
	UPROPERTY()
	TArray<TObjectPtr<UObsidianStashTabWidget>> StashTabs;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> StashTabsList_ScrollBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> StashTab_Overlay;
	
private:
	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemsWidgetController> InventoryItemsWidgetController;
};

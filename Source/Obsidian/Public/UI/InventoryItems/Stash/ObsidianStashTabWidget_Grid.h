// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "UI/InventoryItems/Stash/ObsidianStashTabWidget.h"

#include "ObsidianStashTabWidget_Grid.generated.h"

class UObInventoryItemsWidgetController;
class UObsidianGridPanel;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianStashTabWidget_Grid : public UObsidianStashTabWidget
{
	GENERATED_BODY()

public:
	void InitializeStashTab(UObInventoryItemsWidgetController* InInventoryItemsWidgetController, const int32 InGridWidth,
		const int32 InGridHeight, const FGameplayTag& InStashTabTag);

	virtual void AddItemToStash(UObsidianItem* InItemWidget, const FObsidianItemWidgetData& InItemWidgetData) override;
	virtual void HandleItemChanged(const FObsidianItemWidgetData& InItemWidgetData) override;
	virtual void HandleItemRemoved(const FObsidianItemWidgetData& InItemWidgetData) override;

	virtual void HandleHighlightingItems(const TArray<FObsidianItemPosition>& InItemsToHighlight) override;
	virtual void ClearUsableItemHighlight() override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianGridPanel> StashTab_GridPanel;

private:
	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsController;
};

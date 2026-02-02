// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>
#include <GameplayTagContainer.h>


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
	void InitializeStashTab(UObInventoryItemsWidgetController* InventoryItemsWidgetController, const int32 GridWidth,
		const int32 GridHeight, const FGameplayTag& InStashTabTag);

	virtual void AddItemToStash(UObsidianItem* InItemWidget, const FObsidianItemWidgetData& ItemWidgetData) override;
	virtual void HandleItemChanged(const FObsidianItemWidgetData& ItemWidgetData) override;
	virtual void HandleItemRemoved(const FObsidianItemWidgetData& ItemWidgetData) override;

	virtual void HandleHighlightingItems(const TArray<FObsidianItemPosition>& ItemsToHighlight) override;
	virtual void ClearUsableItemHighlight() override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianGridPanel> StashTab_GridPanel;

private:
	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsController;
};

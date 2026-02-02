// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>
#include <GameplayTagContainer.h>


#include "UI/InventoryItems/Stash/ObsidianStashTabWidget.h"
#include "ObsidianStashTabWidget_Slots.generated.h"

class UObsidianSlotPanel;
class UObsidianPlayerStashWidget;
class UObsidianSlot_ItemSlot;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianStashTabWidget_Slots : public UObsidianStashTabWidget
{
	GENERATED_BODY()

public:
	void InitializeStashTab(UObInventoryItemsWidgetController* InInventoryItemsWidgetController,
		const FGameplayTag& InStashTabTag);

	TArray<UObsidianSlot_ItemSlot*> GetSlotWidgets() const;
	
	virtual void AddItemToStash(UObsidianItem* InItemWidget, const FObsidianItemWidgetData& ItemWidgetData) override;
	virtual void HandleItemChanged(const FObsidianItemWidgetData& ItemWidgetData) override;
	virtual void HandleItemRemoved(const FObsidianItemWidgetData& ItemWidgetData) override;

	virtual void HandleHighlightingItems(const TArray<FObsidianItemPosition>& ItemsToHighlight) override;
	virtual void ClearUsableItemHighlight() override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianSlotPanel> StashTab_SlotPanel;
	
private:
	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsController;
	
};

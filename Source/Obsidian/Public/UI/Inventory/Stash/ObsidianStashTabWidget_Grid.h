// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project


#include "UI/Inventory/Stash/ObsidianStashTabWidget.h"
#include "ObsidianStashTabWidget_Grid.generated.h"

class UObsidianInventoryItemsWidgetController;
class UObsidianGrid;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianStashTabWidget_Grid : public UObsidianStashTabWidget
{
	GENERATED_BODY()

public:
	void InitializeStashTab(UObsidianInventoryItemsWidgetController* WidgetController, const int32 GridWidth, const int32 GridHeight, const FGameplayTag& InStashTabTag);

	virtual void AddItemToStash(UObsidianItem* InItemWidget, const float ItemSlotPadding) override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianGrid> StashTabGrid;

private:
	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemsWidgetController> InventoryItemsController;
	
private:
	void RequestAddingItemToStashTab(const FIntPoint& ToPosition, const bool bShiftDown) const;
};

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

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
	void InitializeStashTab(UObsidianInventoryItemsWidgetController* WidgetController, const int32 GridWidth, const int32 GridHeight);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianGrid> StashTabGrid;
};

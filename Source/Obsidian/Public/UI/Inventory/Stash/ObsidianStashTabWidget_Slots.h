// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>
#include <GameplayTagContainer.h>


#include "UI/Inventory/Stash/ObsidianStashTabWidget.h"
#include "ObsidianStashTabWidget_Slots.generated.h"

class UObsidianSlotPanel;
class UObsidianPlayerStashWidget;
class UObsidianItemSlot_Equipment;

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

	TArray<UObsidianItemSlot_Equipment*> GetSlotWidgets() const;
	
	virtual void AddItemToStash(UObsidianItem* InItemWidget, const float ItemSlotPadding) override;
	
protected:
	void OnStashSlotHover(UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered);
	void OnStashSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot,
		const FObsidianItemInteractionFlags& InteractionFlags) const;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianSlotPanel> StashTab_SlotPanel;
	
private:
	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsController;
	
};

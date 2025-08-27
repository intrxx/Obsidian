// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project


#include "UI/Inventory/Stash/ObsidianStashTabWidget.h"
#include "ObsidianStashTabWidget_Slots.generated.h"

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
	void InitializeStashTab(UObsidianInventoryItemsWidgetController* InInventoryItemsWidgetController, UObsidianPlayerStashWidget* InOwningStashWidget, const FGameplayTag& InStashTabTag);

	virtual void AddItemToStash(UObsidianItem* InItemWidget, const float ItemSlotPadding) override;
	
protected:
	UObsidianItemSlot_Equipment* FindEquipmentSlotForTag(const FGameplayTag& Tag) const;
	
	void OnStashSlotHover(const UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered) const;
	void OnStashSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot, const bool bShiftDown) const;
	
private:
	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemsWidgetController> InventoryItemsController;
	
	UPROPERTY()
	TArray<TObjectPtr<UObsidianItemSlot_Equipment>> EquipmentSlots;

	UPROPERTY()
	TObjectPtr<UObsidianPlayerStashWidget> OwningStashWidget;
};

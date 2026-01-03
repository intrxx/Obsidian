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
	void InitializeStashTab(UObInventoryItemsWidgetController* InInventoryItemsWidgetController, UObsidianPlayerStashWidget* InOwningStashWidget, const FGameplayTag& InStashTabTag);

	TArray<UObsidianItemSlot_Equipment*> GetSlotWidgets() const;
	
	virtual void AddItemToStash(UObsidianItem* InItemWidget, const float ItemSlotPadding) override;
	
protected:
	UObsidianItemSlot_Equipment* FindEquipmentSlotForTag(const FGameplayTag& Tag) const;
	
	void OnStashSlotHover(UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered);
	void OnStashSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot,
		const FObsidianItemInteractionFlags& InteractionFlags) const;
	
private:
	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsController;
	
	UPROPERTY()
	TArray<TObjectPtr<UObsidianItemSlot_Equipment>> EquipmentSlots;

	UPROPERTY()
	TObjectPtr<UObsidianPlayerStashWidget> OwningStashWidget;
};

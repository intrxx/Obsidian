// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

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
	void InitializeStashTab(UObsidianPlayerStashWidget* InOwningStashWidget);

protected:
	void OnEquipmentSlotHover(const UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered) const;
	void OnEquipmentSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot) const;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UObsidianItemSlot_Equipment>> EquipmentSlots;

	UPROPERTY()
	TObjectPtr<UObsidianPlayerStashWidget> OwningStashWidget;
};

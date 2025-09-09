// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UI/ObsidianWidgetBase.h"
#include "ObsidianEquipmentPanel.generated.h"

struct FGameplayTag;

class UObsidianInventory;
class UObsidianItemSlot_Equipment;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianEquipmentPanel : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void InitializeEquipmentPanel(UObsidianInventory* InOwningInventory);

	UObsidianItemSlot_Equipment* FindEquipmentSlotWidgetForTag(const FGameplayTag& Tag) const;
	TArray<UObsidianItemSlot_Equipment*> GetSlotWidgets() const;

	void OnEquipmentSlotHover(UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered);
	void OnEquipmentSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot, const bool bShiftDown) const;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UObsidianItemSlot_Equipment>> EquipmentSlots;

	UPROPERTY()
	TWeakObjectPtr<UObsidianInventory> OwningInventory;
};

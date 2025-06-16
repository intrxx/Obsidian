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

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquipmentSlotPressedSignature, const FGameplayTag& SlotTag);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianEquipmentPanel : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void InitializeEquipmentPanel(UObsidianInventory* InOwningInventory);

	UObsidianItemSlot_Equipment* FindEquipmentSlotForTag(const FGameplayTag& Tag) const;

	void OnEquipmentSlotHover(UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered) const;
	void OnEquipmentSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot) const;

public:
	FOnEquipmentSlotPressedSignature OnEquipmentSlotPressedDelegate;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UObsidianItemSlot_Equipment>> EquipmentSlots;

	UPROPERTY()
	TWeakObjectPtr<UObsidianInventory> OwningInventory;
};

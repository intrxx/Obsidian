// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>
#include <GameplayTagContainer.h>

#include "UI/ObsidianWidgetBase.h"
#include "ObsidianEquipmentPanel.generated.h"

struct FObsidianItemInteractionFlags;
struct FGameplayTag;
struct FObsidianItemWidgetData;

class UObsidianInventory;
class UObsidianItem;
class UObsidianSlotBlockadeItem;
class UObsidianItemSlot_Equipment;
class UObInventoryItemsWidgetController;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianEquipmentPanel : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	virtual void HandleWidgetControllerSet() override;

	UObsidianItemSlot_Equipment* FindEquipmentSlotWidgetForTag(const FGameplayTag& Tag) const;
	TArray<UObsidianItemSlot_Equipment*> GetSlotWidgets() const;
	
	bool IsItemWidgetAtEquipmentSlot(const FGameplayTag& AtSlot) const;
	UObsidianItem* GetItemWidgetAtEquipmentSlot(const FGameplayTag& AtSlot) const;
	bool IsBlockadeItemWidgetAtEquipmentSlot(const FGameplayTag& AtSlot) const;
	UObsidianSlotBlockadeItem* GetBlockadeItemWidgetAtEquipmentSlot(const FGameplayTag& AtSlot) const;

	void AddItemWidget(UObsidianItem* ItemWidget, const FObsidianItemWidgetData& ItemWidgetData);
	
	void HandleItemUnequipped(const FGameplayTag& SlotToClearTag, const bool bBlocksOtherSlot);
	
protected:
	void InitializeEquipmentPanel();
	
	void RegisterEquipmentItemWidget(const FGameplayTag& AtSlot, UObsidianItem* ItemWidget, const bool bSwappedWithAnother);
	void RemoveEquipmentItemWidget(const FGameplayTag& AtSlot);

	/** This function takes the primary slot that is causing the other slot to be blocked. */
	void RegisterBlockedEquipmentItemWidget(const FGameplayTag& AtSlot, UObsidianSlotBlockadeItem* ItemWidget,
		const bool bSwappedWithAnother);
	void RemoveBlockedSlotItemWidget(const FGameplayTag& AtSlot);

	void OnEquipmentSlotHover(UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered);
	void OnEquipmentSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot,
		const FObsidianItemInteractionFlags& InteractionFlags) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianSlotBlockadeItem> SlotBlockadeItemClass;
	
private:
	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsWidgetController;
	
	UPROPERTY()
	TArray<TObjectPtr<UObsidianItemSlot_Equipment>> EquipmentSlots;

	UPROPERTY()
	TMap<FGameplayTag, UObsidianItem*> EquippedItemWidgetMap;

	UPROPERTY()
	TMap<FGameplayTag, UObsidianSlotBlockadeItem*> BlockedSlotsWidgetMap;
};

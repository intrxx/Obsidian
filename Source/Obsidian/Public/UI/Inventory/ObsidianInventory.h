// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "UI/ObsidianMainOverlayWidgetBase.h"
#include "ObsidianInventory.generated.h"

struct FObsidianItemWidgetData;
struct FGameplayTag;

class UObsidianSlotBlockadeItem;
class UObsidianItem;
class UObsidianItemSlot;
class UObsidianInventoryItemDefinition;
class UObsidianItemSlot_Equipment;
class UObsidianInventoryItemInstance;
class UGridPanel;
class USizeBox;
class UOverlay;
class UGridSlot;
class UObInventoryItemsWidgetController;
class UObsidianSlotPanel;
class UObsidianGridPanel;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianInventory : public UObsidianMainOverlayWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void HandleWidgetControllerSet() override;
	virtual void NativeDestruct() override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianGridPanel> Inventory_GridPanel;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianSlotPanel> Equipment_SlotPanel;
	
private:
	void OnItemAdded(const FObsidianItemWidgetData& ItemWidgetData);
	void OnItemChanged(const FObsidianItemWidgetData& ItemWidgetData);
	void OnItemRemoved(const FObsidianItemWidgetData& ItemWidgetData);
	
	void OnItemEquipped(const FObsidianItemWidgetData& ItemWidgetData);
	void OnItemUnequipped(const FObsidianItemWidgetData& ItemWidgetData);
	
	void HighlightSlotPlacement(const FGameplayTagContainer& WithTags);
	void StopHighlightSlotPlacement();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianItem> ItemWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsWidgetController;
	
	UPROPERTY()
	TArray<UObsidianItemSlot_Equipment*> CachedHighlightedSlot;
};

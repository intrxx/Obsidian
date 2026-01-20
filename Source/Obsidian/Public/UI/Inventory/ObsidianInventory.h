// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "UI/ObsidianMainOverlayWidgetBase.h"
#include "ObsidianInventory.generated.h"

struct FObsidianItemWidgetData;
struct FGameplayTag;

class UObsidianSlotBlockadeItem;
class UObsidianItem;
class UObsidianSlotBase;
class UObsidianInventoryItemDefinition;
class UObsidianSlot_ItemSlot;
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
	void OnInventoryItemAdded(const FObsidianItemWidgetData& ItemWidgetData);
	void OnInventoryItemChanged(const FObsidianItemWidgetData& ItemWidgetData);
	void OnInventoryItemRemoved(const FObsidianItemWidgetData& ItemWidgetData);
	
	void OnEquipmentItemAdded(const FObsidianItemWidgetData& ItemWidgetData);
	void OnEquipmentItemChanged(const FObsidianItemWidgetData& ItemWidgetData);
	void OnEquipmentItemRemoved(const FObsidianItemWidgetData& ItemWidgetData);
	
	void HighlightSlotPlacement(const FGameplayTagContainer& WithTags);
	void StopHighlightSlotPlacement();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianItem> ItemWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsWidgetController;
	
	UPROPERTY()
	TArray<UObsidianSlot_ItemSlot*> CachedHighlightedSlot;
};

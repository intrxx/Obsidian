// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"
#include "UI/ObsidianMainOverlayWidgetBase.h"

#include "ObsidianInventory.generated.h"

struct FObsidianItemWidgetData;
struct FGameplayTag;

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
	void OnInventoryItemAdded(const FObsidianItemWidgetData& InItemWidgetData);
	void OnInventoryItemChanged(const FObsidianItemWidgetData& InItemWidgetData);
	void OnInventoryItemRemoved(const FObsidianItemWidgetData& InItemWidgetData);
	
	void OnEquipmentItemAdded(const FObsidianItemWidgetData& InItemWidgetData);
	void OnEquipmentItemChanged(const FObsidianItemWidgetData& InItemWidgetData);
	void OnEquipmentItemRemoved(const FObsidianItemWidgetData& InItemWidgetData);

	void OnUsableContextFiredForInventory(const TArray<FObsidianItemPosition>& InMatchingItemPositions);
	void OnUsableContextFiredForEquipment(const TArray<FObsidianItemPosition>& InMatchingItemPositions);
	
	void HighlightSlotPlacement(const FGameplayTagContainer& InWithTags);
	void StopHighlightSlotPlacement();
	void ClearUsableItemHighlight();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianItem> ItemWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsWidgetController;
	
	UPROPERTY()
	TArray<UObsidianSlot_ItemSlot*> CachedHighlightedSlot;
};

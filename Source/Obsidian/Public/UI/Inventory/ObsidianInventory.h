// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
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
class UObsidianEquipmentPanel;
class UObsidianGrid;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianInventory : public UObsidianMainOverlayWidgetBase
{
	GENERATED_BODY()

public:
	virtual void HandleWidgetControllerSet() override;

	bool IsPlayerDraggingItem() const;
	bool CanEquipDraggedItem(const FGameplayTag& ToSlotTag) const;
	bool CanInteractWithEquipment() const;

	void RequestEquippingItem(const FGameplayTag& ToSlot) const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianGrid> InventoryGrid;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianEquipmentPanel> EquipmentPanel;
	
private:
	void RequestAddingItemToInventory(const FIntPoint& ToPosition, const bool bShiftDown) const;
	
	void OnItemEquipped(const FObsidianItemWidgetData& ItemWidgetData);
	void OnItemUnequipped(const FGameplayTag& SlotTag, const bool bBlocksOtherSlot);
	
	/** Function that triggers when automatically adding item. E.g. from the ground when inventory is hidden. */
	void OnItemAdded(const FObsidianItemWidgetData& ItemWidgetData);
	void OnItemChanged(const FObsidianItemWidgetData& ItemWidgetData);
	
	void OnInventoryItemLeftMouseButtonPressed(const UObsidianItem* ItemWidget, const FObsidianItemInteractionFlags& InteractionFlags);
	void OnInventoryItemRightMouseButtonPressed(UObsidianItem* ItemWidget);
	
	void OnInventoryItemMouseEntered(const UObsidianItem* ItemWidget);

	void HighlightSlotPlacement(const FGameplayTagContainer& WithTags);
	void StopHighlightSlotPlacement();

private:
	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsWidgetController;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianItem> ItemWidgetClass;

	UPROPERTY()
	TArray<UObsidianItemSlot_Equipment*> CachedHighlightedSlot;
};

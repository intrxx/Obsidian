// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


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
class UObsidianInventoryWidgetController;
class UObsidianEquipmentPanel;
class UObsidianInventoryGrid;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianInventory : public UObsidianMainOverlayWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void HandleWidgetControllerSet() override;
	
	float GetInventoryWidth() const
	{
		return RootSizeBoxWidth;
	}

	float GetInventoryHeight() const
	{
		return RootSizeBoxHeight;
	}

	bool IsPlayerDraggingItem() const;
	bool GetDraggedItemGridSize(TArray<FIntPoint>& OutItemGridSize) const;
	bool CanPlaceDraggedItem(const FIntPoint& ToHoveredSlotPosition, const TArray<FIntPoint>& ItemGridSize) const;
	bool CanEquipDraggedItem(const FGameplayTag& ToSlotTag) const;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianInventoryGrid> InventoryGrid;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianEquipmentPanel> EquipmentPanel;
	
private:
	void RequestAddingItemToInventory(const FIntPoint& ToPosition, const bool bShiftDown) const;
	void RequestEquippingItem(const FGameplayTag& ToSlot) const;
	
	void OnItemEquipped(const FObsidianItemWidgetData& ItemWidgetData);
	
	/** Function that triggers when automatically adding item. E.g. from the ground when inventory is hidden. */
	void OnItemAdded(const FObsidianItemWidgetData& ItemWidgetData);
	void OnItemChanged(const FObsidianItemWidgetData& ItemWidgetData);
	
	void OnInventoryItemLeftMouseButtonPressed(const UObsidianItem* ItemWidget, const bool bShiftDown);
	void OnInventoryItemRightMouseButtonPressed(UObsidianItem* ItemWidget);
	void OnEquipmentItemLeftMouseButtonPressed(const UObsidianItem* ItemWidget, const bool bShiftDown);
	
	void OnSlotBlockadeItemLeftMouseButtonPressed(const UObsidianSlotBlockadeItem* SlotBlockadeItem);
	void OnSlotBlockadeItemMouseEntered(const UObsidianSlotBlockadeItem* ItemWidget);
	
	void OnEquipmentItemMouseEntered(const UObsidianItem* ItemWidget);
	void OnInventoryItemMouseEntered(const UObsidianItem* ItemWidget);
	void OnItemMouseLeave();


private:
	UPROPERTY()
	TObjectPtr<UObsidianInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianItem> ItemWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianSlotBlockadeItem> SlotBlockadeItemClass;
	
	/** Essentially, height component of inventory size. Use this instead of directly setting it on SizeBox. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	float RootSizeBoxHeight = 920.0f;

	/** Essentially, width component of inventory size. Use this instead of directly setting it on SizeBox. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	float RootSizeBoxWidth = 820.0f;
};

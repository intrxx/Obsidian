// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "ObsidianItemStoragePanelBase.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"
#include "Slots/ObsidianSlot_ItemSlot.h"

#include "ObsidianSlotPanel.generated.h"

struct FObsidianItemInteractionFlags;
struct FGameplayTag;
struct FObsidianItemWidgetData;

class UObsidianInventory;
class UObsidianItem;
class UObsidianSlotBlockadeItem;
class UObsidianSlot_ItemSlot;
class UObInventoryItemsWidgetController;

USTRUCT()
struct FObsidianSlotData
{
	GENERATED_BODY()

public:
	FObsidianSlotData(){}

	bool IsOccupied() const;
	bool IsBlocked() const;
	void AddNewItem(const FObsidianItemPosition& InPosition, UObsidianItem* InItemWidget, const bool bInBlockSlot);
	void Reset();

public:
	UPROPERTY()
	FObsidianItemPosition OriginPosition = FObsidianItemPosition();
	
	UPROPERTY()
	UObsidianSlot_ItemSlot* OwningSlot = nullptr;

	UPROPERTY()
	UObsidianItem* ItemWidget = nullptr;

protected:
	UPROPERTY()
	bool bBlocked = false;
	
	UPROPERTY()
	bool bOccupied = false;
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianSlotPanel : public UObsidianItemStoragePanelBase
{
	GENERATED_BODY()

public:
	bool ConstructEquipmentPanel();
	bool ConstructStashPanel(const FGameplayTag& InStashTabTag);

	TArray<UObsidianSlot_ItemSlot*> GetAllSlots() const;
	UObsidianSlot_ItemSlot* GetSlotByPosition(const FGameplayTag& InAtSlotTag);
	const FObsidianSlotData* GetSlotDataAtGridPosition(const FGameplayTag& InAtSlotTag) const;
	UObsidianItem* GetItemWidgetAtSlot(const FGameplayTag& InAtSlotTag) const;
	bool IsSlotOccupied(const FGameplayTag& InAtSlotTag) const;
	bool IsSlotBlocked(const FGameplayTag& InAtSlotTag) const;
	
	void AddItemWidget(UObsidianItem* InItemWidget, const FObsidianItemWidgetData& InItemWidgetData,
		const bool bInBlockSlot = false);
	void HandleItemRemoved(const FObsidianItemWidgetData& InItemWidgetData);
	void HandleItemChanged(const FObsidianItemWidgetData& InItemWidgetData);

	void HandleHighlightingItems(const TArray<FObsidianItemPosition>& InItemsToHighlight);
	void ClearUsableItemHighlight();
	
protected:
	virtual void HandleWidgetControllerSet() override;
	virtual void NativeDestruct() override;
	
	bool ConstructSlots();
	
	void RegisterSlotItemWidget(const FObsidianItemPosition& InItemPosition, UObsidianItem* InItemWidget,
		const bool bInSwappedWithAnother, const bool bInBlocksSlot = false,
		const FObsidianItemPosition& InItemOriginPosition = FObsidianItemPosition());
	void UnregisterSlotItemWidget(const FGameplayTag& InSlotTag);
	
	void OnItemSlotHover(UObsidianSlot_ItemSlot* InAffectedSlot, const bool bInEntered);
	void OnItemSlotLeftMouseButtonDown(const UObsidianSlot_ItemSlot* InAffectedSlot,
		const FObsidianItemInteractionFlags& InInteractionFlags);
	void OnItemSlotRightMouseButtonDown(const UObsidianSlot_ItemSlot* InAffectedSlot,
		const FObsidianItemInteractionFlags& InInteractionFlags);
	
	void ConstructItemPosition(FObsidianItemPosition& OutItemPosition, const FGameplayTag& InSlotTagOverride) const;
	
private:
	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsWidgetController;
	
	UPROPERTY()
	TMap<FGameplayTag, FObsidianSlotData> SlotDataMap;

	UPROPERTY()
	TArray<TObjectPtr<UObsidianItem>> HighlightedItems;
};

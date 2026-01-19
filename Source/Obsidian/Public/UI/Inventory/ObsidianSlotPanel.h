// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>
#include <GameplayTagContainer.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "ObsidianItemStoragePanelBase.h"
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
	void AddNewItem(const FObsidianItemPosition& InPosition, UObsidianItem* InItemWidget, const bool bBlockSlot);
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
	UObsidianSlot_ItemSlot* GetSlotByPosition(const FGameplayTag& AtSlotTag);
	const FObsidianSlotData* GetSlotDataAtGridPosition(const FGameplayTag& AtSlotTag) const;
	UObsidianItem* GetItemWidgetAtSlot(const FGameplayTag& AtSlotTag) const;
	bool IsSlotOccupied(const FGameplayTag& AtSlotTag) const;
	bool IsSlotBlocked(const FGameplayTag& AtSlotTag) const;
	
	void AddItemWidget(UObsidianItem* ItemWidget, const FObsidianItemWidgetData& ItemWidgetData,
		const bool bBlockSlot = false);
	void HandleItemRemoved(const FObsidianItemWidgetData& ItemWidgetData);
	void HandleItemChanged(const FObsidianItemWidgetData& ItemWidgetData);
	
protected:
	virtual void HandleWidgetControllerSet() override;
	virtual void NativeDestruct() override;
	
	bool ConstructSlots();
	
	void RegisterSlotItemWidget(const FObsidianItemPosition& ItemPosition, UObsidianItem* ItemWidget,
		const bool bSwappedWithAnother, const bool bBlocksSlot = false,
		const FObsidianItemPosition& ItemOriginPosition = FObsidianItemPosition());
	void UnregisterSlotItemWidget(const FGameplayTag& SlotTag);
	
	void OnItemSlotHover(UObsidianSlot_ItemSlot* AffectedSlot, const bool bEntered);
	void OnItemSlotLeftMouseButtonDown(const UObsidianSlot_ItemSlot* AffectedSlot,
		const FObsidianItemInteractionFlags& InteractionFlags);
	void OnItemSlotRightMouseButtonDown(const UObsidianSlot_ItemSlot* AffectedSlot,
		const FObsidianItemInteractionFlags& InteractionFlags);
	
	void ConstructItemPosition(FObsidianItemPosition& ItemPosition, const FGameplayTag& SlotTagOverride) const;
	
private:
	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsWidgetController;
	
	UPROPERTY()
	TMap<FGameplayTag, FObsidianSlotData> SlotDataMap;
};

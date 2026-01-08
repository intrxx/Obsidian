// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>
#include <GameplayTagContainer.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "ObsidianItemStoragePanelBase.h"
#include "ObsidianSlotPanel.generated.h"

struct FObsidianItemInteractionFlags;
struct FGameplayTag;
struct FObsidianItemWidgetData;

class UObsidianInventory;
class UObsidianItem;
class UObsidianSlotBlockadeItem;
class UObsidianItemSlot_Equipment;
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
	UObsidianItemSlot_Equipment* OwningSlot = nullptr;

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

	TArray<UObsidianItemSlot_Equipment*> GetAllSlots() const;
	UObsidianItemSlot_Equipment* GetSlotByPosition(const FGameplayTag& AtSlotTag);
	const FObsidianSlotData* GetSlotDataAtGridPosition(const FGameplayTag& AtSlotTag) const;
	UObsidianItem* GetItemWidgetAtSlot(const FGameplayTag& AtSlotTag) const;
	bool IsSlotOccupied(const FGameplayTag& AtSlotTag) const;
	bool IsSlotBlocked(const FGameplayTag& AtSlotTag) const;
	
	void AddItemWidget(UObsidianItem* ItemWidget, const FObsidianItemWidgetData& ItemWidgetData,
		const bool bBlockSlot = false);
	void HandleItemRemoved(const FObsidianItemWidgetData& ItemWidgetData);
	
protected:
	virtual void HandleWidgetControllerSet() override;
	virtual void NativeDestruct() override;
	
	bool ConstructSlots();
	
	void RegisterSlotItemWidget(const FObsidianItemPosition& ItemPosition, UObsidianItem* ItemWidget,
		const bool bSwappedWithAnother, const bool bBlocksSlot = false,
		const FObsidianItemPosition& ItemOriginPosition = FObsidianItemPosition());
	void UnregisterSlotItemWidget(const FGameplayTag& SlotTag);
	
	void OnEquipmentSlotHover(UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered);
	void OnEquipmentSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot,
		const FObsidianItemInteractionFlags& InteractionFlags);
	
private:
	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsWidgetController;
	
	UPROPERTY()
	TMap<FGameplayTag, FObsidianSlotData> SlotDataMap;
};

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>
#include <GameplayTagContainer.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "UI/ObsidianWidgetBase.h"
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
	void AddNewItem(const FObsidianItemPosition& InPosition, UObsidianItem* InItemWidget, const bool bBlockSlot);
	void Reset();

public:
	UPROPERTY()
	FObsidianItemPosition OriginPosition = FObsidianItemPosition();
	
	UPROPERTY()
	UObsidianItemSlot_Equipment* OwningSlot = nullptr;

	UPROPERTY()
	UObsidianItem* ItemWidget = nullptr;
	
	UPROPERTY()
	bool bBlocked = false;

protected:
	UPROPERTY()
	bool bOccupied = false;
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianSlotPanel : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	virtual void HandleWidgetControllerSet() override;

	TArray<UObsidianItemSlot_Equipment*> GetAllSlots() const;
	UObsidianItemSlot_Equipment* GetSlotByPosition(const FGameplayTag& AtSlotTag);
	const FObsidianSlotData* GetSlotDataAtGridPosition(const FGameplayTag& AtSlotTag) const;
	UObsidianItem* GetItemWidgetAtSlot(const FGameplayTag& AtSlotTag) const;
	bool IsSlotOccupied(const FGameplayTag& AtSlotTag) const;
	bool IsSlotBlocked(const FGameplayTag& AtSlotTag) const;
	
	void AddItemWidget(UObsidianItem* ItemWidget, const FObsidianItemWidgetData& ItemWidgetData,
		const bool bBlockSlot = false);
	void HandleItemUnequipped(const FGameplayTag& SlotToClearTag, const bool bBlocksOtherSlot);
	
protected:
	virtual void NativeDestruct() override;
	
	void InitializeEquipmentPanel();
	
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

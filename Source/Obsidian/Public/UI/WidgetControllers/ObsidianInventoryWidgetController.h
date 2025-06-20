// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UI/ObsidianWidgetControllerBase.h"
#include "ObsidianInventoryWidgetController.generated.h"

struct FObsidianEquipmentChangeMessage;

class AObsidianPlayerController;
class UObsidianItemDescriptionBase;
class UObsidianUnstackSlider;
class UObsidianHeroComponent;
class UObsidianItemWidget;
class UObsidianItem;
class UObsidianDraggedItem;
class UObsidianSlotBlockadeItem;

/**
 * 
 */
USTRUCT()
struct FObsidianItemWidgetData
{
	GENERATED_BODY()
	
public:
	FObsidianItemWidgetData(){};

	/** Return true if the desired slot for this item is a swap slot. Only valid for equipment. */
	bool IsItemForSwapSlot() const;
	
public:
	UPROPERTY()
	TObjectPtr<UTexture2D> ItemImage = nullptr;

	UPROPERTY()
	FIntPoint DesiredPosition = FIntPoint::NoneValue;
	
	UPROPERTY()
	FGameplayTag DesiredSlot = FGameplayTag::EmptyTag;
	
	UPROPERTY()
	FGameplayTag ItemCategory = FGameplayTag::EmptyTag;
	
	UPROPERTY()
	FIntPoint GridSpan = FIntPoint::NoneValue;

	UPROPERTY()
	float ItemSlotPadding = 0.0f;
	
	UPROPERTY()
	int32 StackCount = 0;
	
	UPROPERTY()
	bool bUsable = false;
	
	UPROPERTY()
	bool bSwappedWithAnotherItem = false;
	
	UPROPERTY()
	bool bDoesBlockSisterSlot = false;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemEquippedSignature, const FObsidianItemWidgetData& ItemWidgetData);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedSignature, const FObsidianItemWidgetData& ItemWidgetData);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemChangedSignature, const FObsidianItemWidgetData& ItemWidgetData);

/**
 *  
 */
UCLASS(BlueprintType, Blueprintable)
class OBSIDIAN_API UObsidianInventoryWidgetController : public UObsidianHeroWidgetControllerBase
{
	GENERATED_BODY()

public:
	//~ Start of UObsidianWidgetController
	virtual void OnWidgetControllerSetupCompleted() override;
	//~ End of UObsidianWidgetController
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|InventoryWidgetController")
	void SetInventoryOpened(const bool bInOpened)
	{
		bInventoryOpened = bInOpened;
	}
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|InventoryWidgetController")
	bool IsInventoryOpened() const
	{
		return bInventoryOpened;
	}

	bool IsDescriptionActive() const
	{
		return bDescriptionActive;
	}

	int32 GetInventoryGridWidth() const;
	int32 GetInventoryGridHeight() const;

	void OnInventoryOpen();
	
	bool IsDraggingAnItem() const;
	bool CanPlaceDraggedItem(const FIntPoint& AtGridSlot) const;
	bool CanPlaceDraggedItem(const FIntPoint& AtGridSlot, const FIntPoint& ItemGridSpan) const;
	bool CanInteractWithInventory() const;

	/** Fills the item grid size, returns false if the grid size could not be found, most likely because item is invalid. */
	bool GetDraggedItemGridSpan(FIntPoint& OutItemGridSpan) const;
	
	UObsidianItem* GetItemWidgetAtInventoryGridSlot(const FIntPoint& AtGridSlot) const;
	void RegisterInventoryItemWidget(const FIntPoint& GridSlot, UObsidianItem* ItemWidget);
	void RemoveInventoryItemWidget(const FIntPoint& GridSlot);

	UObsidianItem* GetItemWidgetAtEquipmentSlot(const FGameplayTag& Slot) const;
	void RegisterEquipmentItemWidget(const FGameplayTag& Slot, UObsidianItem* ItemWidget, const bool bSwappedWithAnother);
	
	/** This function takes the primary slot that is causing the other slot to be blocked. */
	void AddBlockedEquipmentItemWidget(const FGameplayTag& PrimarySlot, UObsidianSlotBlockadeItem* ItemWidget, const bool bSwappedWithAnother);
	void RemoveEquipmentItemWidget(const FGameplayTag& Slot);
	void RemoveBlockedSlotItemWidget(const FGameplayTag& Slot);

	bool CanEquipDraggedItem(const FGameplayTag& SlotTag) const;
	bool CanInteractWithEquipment() const;
	
	void RequestAddingItemToInventory(const FIntPoint& ToGridSlot, const bool bShiftDown);
	void RequestEquippingItem(const FGameplayTag& SlotTag);

	void HandleRightClickingOnInventoryItem(const FIntPoint& AtGridSlot, UObsidianItem* ItemWidget);
	void HandleLeftClickingOnInventoryItem(const FIntPoint& AtGridSlot);
	void HandleLeftClickingOnInventoryItemWithShiftDown(const FIntPoint& AtGridSlot, const UObsidianItem* ItemWidget);
	void HandleLeftClickingOnEquipmentItem(const FGameplayTag& SlotTag, const FGameplayTag& EquipSlotTagOverride = FGameplayTag::EmptyTag);

	void HandleHoveringOverInventoryItem(const FIntPoint& AtGridSlot);
	void HandleHoveringOverInventoryItem(const UObsidianItem* ItemWidget);
	void HandleHoveringOverEquipmentItem(const UObsidianItem* ItemWidget);
	void HandleUnhoveringItem();

	void RemoveItemUIElements();

	UObsidianItemDescriptionBase* CreateItemDescriptionForDroppedItem(const UObsidianInventoryItemInstance* Instance);
	UObsidianItemDescriptionBase* CreateItemDescriptionForDroppedItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 CurrentItemStacks);
	
public:
	FOnItemEquippedSignature OnItemEquippedDelegate;
	FOnItemAddedSignature OnItemAddedDelegate;
	FOnItemChangedSignature OnItemChangedDelegate;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianDraggedItem> DraggedItemWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianUnstackSlider> UnstackSliderClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianItemDescriptionBase> ItemDescriptionClass;

	UPROPERTY()
	TObjectPtr<UObsidianUnstackSlider> ActiveUnstackSlider = nullptr;

	UPROPERTY()
	TObjectPtr<UObsidianItemDescriptionBase> ActiveItemDescription = nullptr;

private:
	void StopUsingItem();
	
	void OnInventoryStateChanged(FGameplayTag Channel, const FObsidianInventoryChangeMessage& InventoryChangeMessage);
	void OnEquipmentStateChanged(FGameplayTag Channel, const FObsidianEquipmentChangeMessage& EquipmentChangeMessage);
	
	void HandleTakingOutStacks(const int32 StacksToTake, const FIntPoint& GridSlotPosition);
	
	void RemoveUnstackSlider();
	void RemoveItemDescription();
	void ClearUsableUIContext();
	
	bool CanShowDescription() const;

	FVector2D CalculateUnstackSliderPosition(const UObsidianItem* ItemWidget) const;
	FVector2D CalculateDescriptionPosition(const UObsidianItem* ItemWidget) const;
	FVector2D GetItemUIElementPositionBoundByViewport(const FVector2D& ViewportSize, const FVector2D& ItemPosition, const FVector2D& ItemSize, const FVector2D& UIElementSize) const;

private:
	bool bInventoryOpened = false;

	bool bDescriptionActive = false;
	bool bUnstackSliderActive = false;
	
	UPROPERTY()
	TObjectPtr<UObsidianHeroComponent> OwnerHeroComponent = nullptr;
	
	UPROPERTY()
	TMap<FIntPoint, UObsidianItem*> AddedItemWidgetMap;

	UPROPERTY()
	TMap<FGameplayTag, UObsidianItem*> EquippedItemWidgetMap;

	UPROPERTY()
	TMap<FGameplayTag, UObsidianSlotBlockadeItem*> BlockedSlotsWidgetMap;

	UPROPERTY()
	TArray<UObsidianItem*> CachedItemsMatchingUsableContext;
};


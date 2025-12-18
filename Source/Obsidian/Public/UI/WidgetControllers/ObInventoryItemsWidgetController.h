// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "UI/ObsidianWidgetControllerBase.h"
#include "ObInventoryItemsWidgetController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWidgetController_Items, Log, All);

struct FObsidianEquipmentChangeMessage;
struct FObsidianStashChangeMessage;

class AObsidianPlayerController;
class UObsidianItemDescriptionBase;
class UObsidianUnstackSlider;
class UObsidianPlayerInputManager;
class UObsidianItemWidget;
class UObsidianItem;
class UObsidianDraggedItem;
class UObsidianSlotBlockadeItem;

USTRUCT()
struct FObsidianStashTabWidgetData
{
	GENERATED_BODY()

public:
	
};


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
	FObsidianItemPosition ItemPosition = FObsidianItemPosition();
	
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

/**
 * 
 */
USTRUCT()
struct FObsidianStashAddedItemWidgets
{
	GENERATED_BODY()

public:
	FObsidianStashAddedItemWidgets(){}
	FObsidianStashAddedItemWidgets(const FGameplayTag& InStashTabTag)
		: StashTag(InStashTabTag){}

	FGameplayTag GetStashTag() const;
	int32 GetNumberOfItemsAdded() const;

	UObsidianItem* GetItemWidgetAtPosition(const FObsidianItemPosition& ItemPosition) const;

	/** Tries to add Item widget to internal map, will return false if item position is already occupied. */
	bool AddItemWidget(const FObsidianItemPosition& ItemPosition, UObsidianItem* ItemWidget);
	bool RemoveItemWidget(const FObsidianItemPosition& ItemPosition);

	void EmptyAddedItemWidgets(const int32 OptionalReserve = 0);

	void DebugPrintAllAddedItems();
	
protected:
	UPROPERTY()
	FGameplayTag StashTag = FGameplayTag::EmptyTag;

	UPROPERTY()
	TMap<FObsidianItemPosition, UObsidianItem*> StashAddedItemWidgetsMap;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedSignature, const FObsidianItemWidgetData& ItemWidgetData);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemChangedSignature, const FObsidianItemWidgetData& ItemWidgetData);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStartPlacementHighlightSignature, const FGameplayTagContainer& ForSlotsWithTag);
DECLARE_MULTICAST_DELEGATE(FOnStopPlacementHighlightSignature);

/**
 *  
 */
UCLASS(BlueprintType, Blueprintable)
class OBSIDIAN_API UObInventoryItemsWidgetController : public UObsidianHeroWidgetControllerBase
{
	GENERATED_BODY()

public:
	//~ Start of UObsidianWidgetController
	virtual void OnWidgetControllerSetupCompleted() override;
	//~ End of UObsidianWidgetController
	
	bool IsDescriptionActive() const
	{
		return bDescriptionActive;
	}

	UObsidianItemDescriptionBase* GetActiveItemDescription()
	{
		return ActiveItemDescription;
	}

	TConstArrayView<TObjectPtr<UObsidianStashTab>> GetAllStashTabs() const;

	int32 GetInventoryGridWidth() const;
	int32 GetInventoryGridHeight() const;

	void OnInventoryOpen();
	void OnPlayerStashOpen();
	
	bool IsDraggingAnItem() const;
	bool CanPlaceDraggedItem(const EObsidianGridOwner GridOwner, const FIntPoint& AtGridSlot, const FGameplayTag& StashTag = FGameplayTag::EmptyTag) const;
	bool CanPlaceDraggedItem(const EObsidianGridOwner GridOwner, const FIntPoint& AtGridSlot, const FIntPoint& ItemGridSpan, const FGameplayTag& StashTag = FGameplayTag::EmptyTag) const;
	bool CanPlaceItemAtStashSlot(const FObsidianItemPosition& ItemPosition) const;
	bool CanInteractWithGrid(const EObsidianGridOwner GridOwner) const;
	bool CanInteractWithInventory() const;
	bool CanInteractWithPlayerStash() const;

	/** Fills the item grid size, returns false if the grid size could not be found, most likely because item is invalid. */
	bool GetDraggedItemGridSpan(FIntPoint& OutItemGridSpan) const;
	
	UObsidianItem* GetItemWidgetAtInventoryGridSlot(const FIntPoint& AtGridSlot) const;
	void RegisterInventoryItemWidget(const FIntPoint& GridSlot, UObsidianItem* ItemWidget);
	void RemoveInventoryItemWidget(const FIntPoint& GridSlot);

	FString GetStashTabName(const FGameplayTag StashTabTag) const;

	UObsidianItem* GetItemWidgetAtEquipmentSlot(const FGameplayTag& Slot) const;
	void RegisterEquipmentItemWidget(const FGameplayTag& Slot, UObsidianItem* ItemWidget, const bool bSwappedWithAnother);
	
	void RegisterCurrentStashTab(const FGameplayTag& CurrentStashTab);

	UObsidianItem* GetItemWidgetAtStashPosition(const FObsidianItemPosition& ItemPosition) const;
	void RegisterStashTabItemWidget(const FObsidianItemPosition& ItemPosition, UObsidianItem* ItemWidget);
	void RemoveStashItemWidget(const FObsidianItemPosition& ItemPosition);
	
	/** This function takes the primary slot that is causing the other slot to be blocked. */
	void AddBlockedEquipmentItemWidget(const FGameplayTag& PrimarySlot, UObsidianSlotBlockadeItem* ItemWidget, const bool bSwappedWithAnother);
	void RemoveEquipmentItemWidget(const FGameplayTag& Slot);
	void RemoveBlockedSlotItemWidget(const FGameplayTag& Slot);

	bool CanEquipDraggedItem(const FGameplayTag& SlotTag) const;
	bool CanInteractWithEquipment() const;
	
	void RequestAddingItemToInventory(const FIntPoint& ToGridSlot, const bool bShiftDown);
	void RequestEquippingItem(const FGameplayTag& SlotTag);
	void RequestAddingItemToStashTab(const FObsidianItemPosition& ToPosition, const bool bShiftDown);

	void HandleRightClickingOnInventoryItem(const FIntPoint& AtGridSlot, UObsidianItem* ItemWidget);
	void HandleLeftClickingOnInventoryItem(const FIntPoint& AtGridSlot, const bool bAddToOtherWindow);
	void HandleLeftClickingOnInventoryItemWithShiftDown(const FIntPoint& AtGridSlot, const UObsidianItem* ItemWidget);
	
	void HandleLeftClickingOnEquipmentItem(const FGameplayTag& SlotTag, const FGameplayTag& EquipSlotTagOverride = FGameplayTag::EmptyTag);

	void HandleRightClickingOnStashedItem(const FObsidianItemPosition& AtItemPosition, UObsidianItem* ItemWidget);
	void HandleLeftClickingOnStashedItem(const FObsidianItemPosition& AtItemPosition, const bool bAddToOtherWindow);
	void HandleLeftClickingOnStashedItemWithShiftDown(const FObsidianItemPosition& AtItemPosition, const UObsidianItem* ItemWidget);

	void HandleHoveringOverInventoryItem(const FIntPoint& AtGridSlot);
	void HandleHoveringOverInventoryItem(const UObsidianItem* ItemWidget);
	void HandleHoveringOverEquipmentItem(const UObsidianItem* ItemWidget);
	void HandleHoveringOverStashedItem(const UObsidianItem* ItemWidget);
	void HandleUnhoveringItem();

	void RemoveItemUIElements();
	void RemoveCurrentItemDescription();

	void CreateItemDescriptionForDroppedItem(const UObsidianInventoryItemInstance* Instance);
	void CreateItemDescriptionForDroppedItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FObsidianItemGeneratedData& ItemGeneratedData);
	
public:
	FOnItemAddedSignature OnItemEquippedDelegate;
	FOnItemAddedSignature OnItemAddedDelegate;
	FOnItemAddedSignature OnItemStashedDelegate;
	
	FOnItemChangedSignature OnInventoryItemChangedDelegate;
	FOnItemChangedSignature OnStashedItemChangedDelegate;

	/** As of now this delegate will fire once with all Slot Tags that are possible to add the Dragged Item to and it is on individual Widget side to parse these. */
	FOnStartPlacementHighlightSignature OnStartPlacementHighlightDelegate;
	FOnStopPlacementHighlightSignature OnStopPlacementHighlightDelegate;

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
	void OnPlayerStashChanged(FGameplayTag Channel, const FObsidianStashChangeMessage& StashChangeMessage);

	void OnStartDraggingItem(const FDraggedItem& DraggedItem);
	void OnStopDraggingItem();
	
	void HandleTakingOutStacksFromInventory(const int32 StacksToTake, const FObsidianItemPosition& ItemPosition);
	void HandleTakingOutStacksFromStash(const int32 StacksToTake, const FObsidianItemPosition& ItemPosition);
	
	void RemoveUnstackSlider();
	void ClearUsableUIContext();
	
	bool CanShowDescription() const;

	FVector2D CalculateUnstackSliderPosition(const UObsidianItem* ItemWidget) const;
	FVector2D CalculateDescriptionPosition(const UObsidianItem* ItemWidget) const;
	FVector2D GetItemUIElementPositionBoundByViewport(const FVector2D& ViewportSize, const FVector2D& ItemPosition, const FVector2D& ItemSize, const FVector2D& UIElementSize) const;

	UObsidianItemDescriptionBase* CreateInventoryItemDescription(const UObsidianItem* ForItemWidget, const FObsidianItemStats& ItemStats);
	UObsidianItemDescriptionBase* CreateDroppedItemDescription(const FObsidianItemStats& ItemStats);

	void RegisterInitialStashTabs();
	void EmptyRegisteredItems();

private:
	bool bDescriptionActive = false;
	bool bUnstackSliderActive = false;
	
	UPROPERTY()
	TObjectPtr<UObsidianPlayerInputManager> OwnerPlayerInputManager = nullptr;
	
	UPROPERTY()
	TMap<FIntPoint, UObsidianItem*> AddedItemWidgetMap;

	UPROPERTY()
	TMap<FGameplayTag, UObsidianItem*> EquippedItemWidgetMap;

	UPROPERTY()
	TArray<FObsidianStashAddedItemWidgets> StashAddedItemWidgets;

	UPROPERTY()
	TMap<FGameplayTag, UObsidianSlotBlockadeItem*> BlockedSlotsWidgetMap;

	UPROPERTY()
	TArray<UObsidianItem*> CachedItemsMatchingUsableContext;
};


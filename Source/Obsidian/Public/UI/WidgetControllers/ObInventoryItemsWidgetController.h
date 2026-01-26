// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "UI/ObsidianWidgetControllerBase.h"
#include "ObInventoryItemsWidgetController.generated.h"

struct FObsidianEquipmentChangeMessage;
struct FObsidianStashChangeMessage;

class UObsidianItemManagerComponent;
class AObsidianPlayerController;
class UObsidianItemDescriptionBase;
class UObsidianUnstackSlider;
class UObsidianItemWidget;
class UObsidianItem;
class UObsidianDraggedItem;
class UObsidianSlotBlockadeItem;

DECLARE_LOG_CATEGORY_EXTERN(LogWidgetController_Items, Log, All);

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
	uint8 StackCount = 0;
	
	UPROPERTY()
	bool bUsable = false;
	
	UPROPERTY()
	bool bSwappedWithAnotherItem = false;
	
	UPROPERTY()
	bool bDoesBlockSisterSlot = false;
	
	/**
	 * Change Flags
	 */

	UPROPERTY()
	uint8 bUpdateStacks:1 = false;

	UPROPERTY()
	uint8 bGeneralItemUpdate:1 = false;
};

USTRUCT()
struct FObsidianActiveItemDescriptionData
{
	GENERATED_BODY()

public:
	FObsidianActiveItemDescriptionData(){}
	FObsidianActiveItemDescriptionData(UObsidianItemDescriptionBase* InItemDesc, const EObsidianPanelOwner InPanelOwner)
		: OwningItemDescription(InItemDesc)
		, DescriptionPanelOwner(InPanelOwner)
	{}
	
public:
	UPROPERTY()
	TObjectPtr<UObsidianItemDescriptionBase> OwningItemDescription = nullptr;

	UPROPERTY()
	EObsidianPanelOwner DescriptionPanelOwner = EObsidianPanelOwner::None;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedSignature, const FObsidianItemWidgetData& ItemWidgetData);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemChangedSignature, const FObsidianItemWidgetData& ItemWidgetData);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemovedSignature, const FObsidianItemWidgetData& ItemWidgetData);

using MatchingStashedItemsContainer = const TMultiMap<FGameplayTag, FObsidianItemPosition>&;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUsableContextFiredForStashSignature, MatchingStashedItemsContainer ItemsMatchingContext);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUsableContextFiredSignature, const TArray<FObsidianItemPosition>& ItemsMatchingContext);

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
	
	UObsidianItemDescriptionBase* GetActiveDroppedItemDescription();
	
	TConstArrayView<TObjectPtr<UObsidianStashTab>> GetAllStashTabs() const;
	FString GetStashTabName(const FGameplayTag StashTabTag) const;
	
	int32 GetInventoryGridWidth() const;
	int32 GetInventoryGridHeight() const;
	
	bool IsDraggingAnItem() const;
	FIntPoint GetDraggedItemGridSpan() const;
	FIntPoint GetItemGridSpanByPosition(const FObsidianItemPosition& ItemPosition) const;
	
	bool CanInteractWithGrid(const EObsidianPanelOwner PanelOwner) const;
	bool CanInteractWithSlots(const EObsidianPanelOwner PanelOwner) const;
	bool CanInteractWithInventory() const;
	bool CanInteractWithEquipment() const;
	bool CanInteractWithPlayerStash() const;
	
	bool CanPlaceDraggedItemAtPosition(const FObsidianItemPosition& AtPosition,
		const EObsidianPanelOwner PanelOwner) const;
	
	void HandleLeftClickingOnSlot(const FObsidianItemPosition& AtItemPosition,
		const FObsidianItemInteractionData& InteractionData, const EObsidianPanelOwner PanelOwner);
	void HandleRightClickingOnSlot(const FObsidianItemPosition& AtItemPosition,
		const FObsidianItemInteractionData& InteractionData, const EObsidianPanelOwner PanelOwner);
	void HandleRightClickingOnItem(const FObsidianItemPosition& AtItemPosition,
		const FObsidianItemInteractionData& InteractionData, const EObsidianPanelOwner PanelOwner);
	void HandleLeftClickingOnItem(const FObsidianItemPosition& AtItemPosition,
		const FObsidianItemInteractionData& InteractionData, const EObsidianPanelOwner PanelOwner);

	void HandleHoveringOverItem(const FObsidianItemPosition& ItemPosition,
		const FObsidianItemInteractionData& InteractionData, const EObsidianPanelOwner PanelOwner);
	void HandleUnhoveringItem(const FObsidianItemPosition& FromPosition);
	
	void RemoveItemUIElements(const EObsidianPanelOwner ForPanelOwner);
	void RemoveCurrentDroppedItemDescription();

	void CreateItemDescriptionForDroppedItem(const UObsidianInventoryItemInstance* Instance);
	void CreateItemDescriptionForDroppedItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef,
		const FObsidianItemGeneratedData& ItemGeneratedData);

	void RegisterCurrentStashTab(const FGameplayTag& CurrentStashTab);

	void OnInventoryOpen();
	void OnPlayerStashOpen();
	
public:
	FOnItemAddedSignature OnItemEquippedDelegate;
	FOnItemAddedSignature OnItemInventorizedDelegate;
	FOnItemAddedSignature OnItemStashedDelegate;

	FOnItemChangedSignature OnEquippedItemChangedDelegate;
	FOnItemChangedSignature OnInventoryItemChangedDelegate;
	FOnItemChangedSignature OnStashedItemChangedDelegate;

	FOnItemRemovedSignature OnEquippedItemRemovedDelegate;
	FOnItemRemovedSignature OnInventorizedItemRemovedDelegate;
	FOnItemRemovedSignature OnStashedItemRemovedDelegate;

	FOnUsableContextFiredSignature OnUsableContextFiredForInventoryDelegate;
	FOnUsableContextFiredSignature OnUsableContextFiredForEquipmentDelegate;
	FOnUsableContextFiredForStashSignature OnUsableContextFiredForStashDelegate;

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

private:
	void OnInventoryStateChanged(FGameplayTag Channel, const FObsidianInventoryChangeMessage& InventoryChangeMessage);
	void OnEquipmentStateChanged(FGameplayTag Channel, const FObsidianEquipmentChangeMessage& EquipmentChangeMessage);
	void OnPlayerStashChanged(FGameplayTag Channel, const FObsidianStashChangeMessage& StashChangeMessage);

	bool CanPlaceDraggedItemInInventory(const FIntPoint& AtGridSlot) const;
	bool CanPlaceDraggedItemInStash(const FObsidianItemPosition& ItemPosition) const;
	bool CanPlaceDraggedItemInEquipment(const FGameplayTag& SlotTag) const;

	bool CanShowDescription() const;

	void RequestAddingItem(const FObsidianItemPosition& AtItemPosition,
		const FObsidianItemInteractionData& InteractionData, const EObsidianPanelOwner PanelOwner);
	void RequestAddingItemToInventory(const FIntPoint& ToGridSlot, const bool bShiftDown);
	void RequestAddingItemToEquipment(const FGameplayTag& SlotTag);
	void RequestAddingItemToStashTab(const FObsidianItemPosition& ToPosition, const bool bShiftDown);

	void HandleLeftClickingOnInventoryItem(const FIntPoint& ClickedItemPosition, const FIntPoint& ClickedGridPosition,
		const bool bAddToOtherWindow);
	void HandleLeftClickingOnInventoryItemWithShiftDown(const FIntPoint& ClickedItemPosition, const UObsidianItem* ItemWidget);
	void HandleLeftClickingOnEquipmentItem(const FGameplayTag& SlotTag,
		const FGameplayTag& EquipSlotTagOverride = FGameplayTag::EmptyTag);
	void HandleLeftClickingOnStashedItem(const FObsidianItemPosition& AtItemPosition, const bool bAddToOtherWindow);
	void HandleLeftClickingOnStashedItemWithShiftDown(const FObsidianItemPosition& AtItemPosition,
		const UObsidianItem* ItemWidget);

	void HandleRightClickingOnInventoryItem(const FIntPoint& AtGridSlot, UObsidianItem* ItemWidget);
	void HandleRightClickingOnStashedItem(const FObsidianItemPosition& AtItemPosition, UObsidianItem* ItemWidget);
	
	void OnStartDraggingItem(const FDraggedItem& DraggedItem);
	void OnStopDraggingItem();
	
	void HandleTakingOutStacksFromInventory(const int32 StacksToTake, const FObsidianItemPosition& ItemPosition);
	void HandleTakingOutStacksFromStash(const int32 StacksToTake, const FObsidianItemPosition& ItemPosition);
	
	void RemoveUnstackSlider();
	void ClearItemDescriptionForPosition(const FObsidianItemPosition& ForPosition);
	void ClearItemDescriptionsForOwner(const EObsidianPanelOwner ForDescriptionOwner);

	UObsidianItemDescriptionBase* CreateInventoryItemDescription(const FObsidianItemPosition& AtPosition,
		const EObsidianPanelOwner PanelOwner, const UObsidianItem* ForItemWidget, const FObsidianItemStats& ItemStats);
	UObsidianItemDescriptionBase* CreateDroppedItemDescription(const FObsidianItemStats& ItemStats);
	
	FVector2D CalculateUnstackSliderPosition(const UObsidianItem* ItemWidget) const;
	FVector2D CalculateDescriptionPosition(const UObsidianItem* ItemWidget, UObsidianItemDescriptionBase* ForDescription) const;
	FVector2D GetItemUIElementPositionBoundByViewport(const FVector2D& ViewportSize, const FVector2D& ItemPosition,
		const FVector2D& ItemSize, const FVector2D& UIElementSize) const;

private:
	UPROPERTY()
	TWeakObjectPtr<UObsidianInventoryComponent> OwnerInventoryComponent = nullptr;
	UPROPERTY()
	TWeakObjectPtr<UObsidianEquipmentComponent> OwnerEquipmentComponent = nullptr;
	UPROPERTY()
	TWeakObjectPtr<UObsidianPlayerStashComponent> OwnerPlayerStashComponent = nullptr;
	UPROPERTY()
	TWeakObjectPtr<UObsidianCraftingComponent> OwnerCraftingComponent = nullptr;
	UPROPERTY()
	TWeakObjectPtr<UObsidianItemManagerComponent> OwnerItemManagerComponent = nullptr;
	
	UPROPERTY()
	TArray<UObsidianItem*> CachedItemsMatchingUsableContext;

	UPROPERTY()
	TObjectPtr<UObsidianUnstackSlider> ActiveUnstackSlider = nullptr;
	bool bUnstackSliderActive = false;
	
	UPROPERTY()
	TObjectPtr<UObsidianItemDescriptionBase> ActiveDroppedItemDescription = nullptr;
	bool bDroppedDescriptionActive = false;
	
	UPROPERTY()
	TMap<FObsidianItemPosition, FObsidianActiveItemDescriptionData> ActiveItemDescriptions;
};


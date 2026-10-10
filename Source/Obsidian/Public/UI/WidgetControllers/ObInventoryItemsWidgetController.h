// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

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
	FString GetStashTabName(const FGameplayTag InStashTabTag) const;
	
	int32 GetInventoryGridWidth() const;
	int32 GetInventoryGridHeight() const;
	
	bool IsDraggingAnItem() const;
	FIntPoint GetDraggedItemGridSpan() const;
	FIntPoint GetItemGridSpanByPosition(const FObsidianItemPosition& InItemPosition) const;
	
	bool CanInteractWithGrid(const EObsidianPanelOwner InPanelOwner) const;
	bool CanInteractWithSlots(const EObsidianPanelOwner InPanelOwner) const;
	bool CanInteractWithInventory() const;
	bool CanInteractWithEquipment() const;
	bool CanInteractWithPlayerStash() const;
	
	bool CanPlaceDraggedItemAtPosition(const FObsidianItemPosition& InAtPosition,
		const EObsidianPanelOwner InPanelOwner) const;
	
	void HandleLeftClickingOnSlot(const FObsidianItemPosition& InAtItemPosition,
		const FObsidianItemInteractionData& InInteractionData, const EObsidianPanelOwner InPanelOwner);
	void HandleRightClickingOnSlot(const FObsidianItemPosition& InAtItemPosition,
		const FObsidianItemInteractionData& InInteractionData, const EObsidianPanelOwner InPanelOwner);
	void HandleRightClickingOnItem(const FObsidianItemPosition& InAtItemPosition,
		const FObsidianItemInteractionData& InInteractionData, const EObsidianPanelOwner InPanelOwner);
	void HandleLeftClickingOnItem(const FObsidianItemPosition& InAtItemPosition,
		const FObsidianItemInteractionData& InInteractionData, const EObsidianPanelOwner InPanelOwner);

	void HandleHoveringOverItem(const FObsidianItemPosition& InItemPosition,
		const FObsidianItemInteractionData& InInteractionData, const EObsidianPanelOwner InPanelOwner);
	void HandleUnhoveringItem(const FObsidianItemPosition& InFromPosition);
	
	void RemoveItemUIElements(const EObsidianPanelOwner InForPanelOwner);
	void RemoveCurrentDroppedItemDescription();

	void CreateItemDescriptionForDroppedItem(const UObsidianInventoryItemInstance* InInstance);
	void CreateItemDescriptionForDroppedItem(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef,
		const FObsidianItemGeneratedData& InItemGeneratedData);

	void RegisterCurrentStashTab(const FGameplayTag& InCurrentStashTab);

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
	void OnInventoryStateChanged(FGameplayTag InChannel, const FObsidianInventoryChangeMessage& InInventoryChangeMessage);
	void OnEquipmentStateChanged(FGameplayTag InChannel, const FObsidianEquipmentChangeMessage& InEquipmentChangeMessage);
	void OnPlayerStashChanged(FGameplayTag InChannel, const FObsidianStashChangeMessage& InStashChangeMessage);

	bool CanPlaceDraggedItemInInventory(const FIntPoint& InAtGridSlot) const;
	bool CanPlaceDraggedItemInStash(const FObsidianItemPosition& InItemPosition) const;
	bool CanPlaceDraggedItemInEquipment(const FGameplayTag& InSlotTag) const;

	bool CanShowDescription() const;

	void RequestAddingItem(const FObsidianItemPosition& InAtItemPosition,
		const FObsidianItemInteractionData& InInteractionData, const EObsidianPanelOwner InPanelOwner);
	void RequestAddingItemToInventory(const FIntPoint& InToGridSlot, const bool bInShiftDown);
	void RequestAddingItemToEquipment(const FGameplayTag& InSlotTag);
	void RequestAddingItemToStashTab(const FObsidianItemPosition& InToPosition, const bool bInShiftDown);

	void HandleLeftClickingOnInventoryItem(const FIntPoint& InClickedItemPosition, const FIntPoint& InClickedGridPosition,
		const bool bInAddToOtherWindow);
	void HandleLeftClickingOnInventoryItemWithShiftDown(const FIntPoint& InClickedItemPosition, const UObsidianItem* InItemWidget);
	void HandleLeftClickingOnEquipmentItem(const FGameplayTag& InSlotTag,
		const FGameplayTag& InEquipSlotTagOverride = FGameplayTag::EmptyTag);
	void HandleLeftClickingOnStashedItem(const FObsidianItemPosition& InAtItemPosition, const bool bInAddToOtherWindow);
	void HandleLeftClickingOnStashedItemWithShiftDown(const FObsidianItemPosition& InAtItemPosition,
		const UObsidianItem* InItemWidget);

	void HandleRightClickingOnInventoryItem(const FIntPoint& InAtGridSlot, UObsidianItem* InItemWidget);
	void HandleRightClickingOnStashedItem(const FObsidianItemPosition& InAtItemPosition, UObsidianItem* InItemWidget);
	
	void OnStartDraggingItem(const FDraggedItem& InDraggedItem);
	void OnStopDraggingItem();
	
	void HandleTakingOutStacksFromInventory(const int32 InStacksToTake, const FObsidianItemPosition& InItemPosition);
	void HandleTakingOutStacksFromStash(const int32 InStacksToTake, const FObsidianItemPosition& InItemPosition);
	
	void RemoveUnstackSlider();
	void ClearItemDescriptionForPosition(const FObsidianItemPosition& InForPosition);
	void ClearItemDescriptionsForOwner(const EObsidianPanelOwner InForDescriptionOwner);

	UObsidianItemDescriptionBase* CreateInventoryItemDescription(const FObsidianItemPosition& InAtPosition,
		const EObsidianPanelOwner InPanelOwner, const UObsidianItem* InForItemWidget, const FObsidianItemStats& InItemStats);
	UObsidianItemDescriptionBase* CreateDroppedItemDescription(const FObsidianItemStats& InItemStats);
	
	FVector2D CalculateUnstackSliderPosition(const UObsidianItem* InItemWidget) const;
	FVector2D CalculateDescriptionPosition(const UObsidianItem* InItemWidget, UObsidianItemDescriptionBase* InForDescription) const;
	FVector2D GetItemUIElementPositionBoundByViewport(const FVector2D& InViewportSize, const FVector2D& InItemPosition,
		const FVector2D& InItemSize, const FVector2D& InUIElementSize) const;

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


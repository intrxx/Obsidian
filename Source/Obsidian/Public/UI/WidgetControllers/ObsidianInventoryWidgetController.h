// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetControllerBase.h"
#include "ObsidianInventoryWidgetController.generated.h"

class AObsidianPlayerController;
class UObsidianItemDescriptionBase;
class UObsidianUnstackSlider;
class UObsidianHeroComponent;
class UObsidianItemWidget;
class UObsidianItem;
class UObsidianDraggedItem;

struct FObsidianItemVisuals
{
public:
	FObsidianItemVisuals(){};
	FObsidianItemVisuals(UTexture2D* InItemImage, const FVector2D& InDesiredPosition, const FVector2D& InGridSpan, const int32 InStackCount)
		: ItemImage(InItemImage)
		, DesiredPosition(InDesiredPosition)
		, GridSpan(InGridSpan)
		, StackCount(InStackCount)
	{};
	
public:
	UTexture2D* ItemImage = nullptr;
	FVector2D DesiredPosition = FVector2D::Zero();
	FVector2D GridSpan = FVector2D::Zero();
	int32 StackCount = 0;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedSignature, const FObsidianItemVisuals& ItemVisuals);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemChangedSignature, const FObsidianItemVisuals& NewItemVisuals);

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
	
	bool IsDraggingAnItem() const;
	bool CanPlaceDraggedItem(const FVector2D& HoveredSlot, const TArray<FVector2D>& ItemGridSize = TArray<FVector2D>()) const;

	/** Fills the item grid size, returns false if the grid size could not be found, most likely because item is invalid. */
	bool GetDraggedItemGridSize(TArray<FVector2D>& OutItemGridSize) const;

	UObsidianItem* GetItemWidgetAtLocation(const FVector2D& Location) const;
	void AddItemWidget(const FVector2D& Location, UObsidianItem* ItemWidget);
	void RemoveItemWidget(const FVector2D& Location);
	
	void OnInventoryOpen();

	void RequestAddingItemToInventory(const FVector2D& SlotPosition, const bool bShiftDown);
	
	void HandleLeftClickingOnAnItem(const FVector2D& SlotPosition, UObsidianItem* ItemWidget);
	void HandleLeftClickingOnAnItemWithShiftDown(const FVector2D& SlotPosition, UObsidianItem* ItemWidget);
	void HandleHoveringOverItem(const FVector2D& SlotPosition, const UObsidianItem* ItemWidget);
	void HandleUnhoveringItem(const FVector2D& SlotPosition);

	void RemoveItemUIElements();

	UObsidianItemDescriptionBase* CreateItemDescriptionForDroppedItem(const UObsidianInventoryItemInstance* Instance);
	UObsidianItemDescriptionBase* CreateItemDescriptionForDroppedItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 CurrentItemStacks);
	
public:
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
	void OnInventoryStateChanged(FGameplayTag Channel, const FObsidianInventoryChangeMessage& InventoryChangeMessage);
	
	void HandleTakingOutStacks(UObsidianInventoryItemInstance* ItemInstance, const FVector2D& SlotPosition, UObsidianItem* ItemWidget, const int32 CurrentStacks, const int32 StacksToTake);
	
	void RemoveUnstackSlider();
	void RemoveItemDescription();

	bool CanShowDescription() const;

	FVector2D CalculateUnstackSliderPosition(const UObsidianItem* ItemWidget) const;
	FVector2D CalculateDescriptionPosition(const UObsidianItem* ItemWidget) const;
	FVector2D GetItemUIElementPositionBoundByViewport(const FVector2D& ViewportSize, const FVector2D& ItemPosition, const FVector2D& ItemSize, const FVector2D& UIElementSize) const;

private:
	bool bInventoryOpened = false;

	bool bDescriptionActive = false;
	bool bUnstackSliderActive = false;

	UPROPERTY()
	TObjectPtr<UObsidianInventoryComponent> OwnerInventoryComponent = nullptr;

	UPROPERTY()
	TObjectPtr<UObsidianHeroComponent> OwnerHeroComponent = nullptr;

	UPROPERTY()
	TObjectPtr<AObsidianPlayerController> ObsidianPC = nullptr;
	
	UPROPERTY()
	TMap<FVector2D, UObsidianItem*> AddedItemWidgetMap;
};

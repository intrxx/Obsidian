// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetControllerBase.h"
#include "ObsidianInventoryWidgetController.generated.h"

class UObsidianHeroComponent;
class UObsidianItemWidget;
class UObsidianItem;
class UObsidianDraggedItem;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnItemAddedSignature, UTexture2D* ItemImage, const FVector2D& DesiredPosition, const FVector2D& GridSpan);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class OBSIDIAN_API UObsidianInventoryWidgetController : public UObsidianHeroWidgetControllerBase
{
	GENERATED_BODY()

public:
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
	
	bool IsDraggingAnItem() const;
	bool CanPlaceDraggedItem(const FVector2D& HoveredSlot, const TArray<FVector2D>& ItemGridSize = TArray<FVector2D>()) const;

	/** Fills the item grid size, returns false if the grid size could not be found, most likely because item is invalid. */
	bool GetDraggedItemGridSize(TArray<FVector2D>& OutItemGridSize) const;

	void AddItemWidget(const FVector2D& Location, UObsidianItem* ItemWidget);
	void RemoveItemWidget(const FVector2D& Location);
	
	//~ Start of UObsidianWidgetController
	virtual void OnWidgetControllerSetupCompleted() override;
	//~ End of UObsidianWidgetController

	void OnItemAdded(UObsidianInventoryItemInstance* ItemInstance, const FVector2D DesiredPosition);
	void OnInventoryOpen();

	void RequestAddingItemToInventory(const FVector2D& SlotPosition);
	void RequestPickingUpItemFromInventory(const FVector2D& SlotPosition);

public:
	FOnItemAddedSignature OnItemAddedDelegate;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianDraggedItem> DraggedItemWidgetClass;

private:
	bool CanAddToSpecificSlot(const TArray<FVector2D>& ItemGridSize, const FVector2D& HoveredSlot) const;
	void PickupItem(const FVector2D& SlotPosition);
	
private:
	TMap<FVector2D, UObsidianInventoryItemInstance*> GridLocationToItemMap;
	TMap<FVector2D, bool> InventoryStateMap;
	bool bInventoryOpened = false;
	//bool bInventoryChanged = false;

	UPROPERTY()
	TObjectPtr<UObsidianInventoryComponent> InternalInventoryComponent;

	UPROPERTY()
	TObjectPtr<UObsidianHeroComponent> InternalHeroComponent;
	
	UPROPERTY()
	TMap<FVector2D, UObsidianItem*> AddedItemWidgetMap;
};

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianMainOverlayWidgetBase.h"
#include "ObsidianInventory.generated.h"

struct FObsidianItemVisuals;
class UObsidianItem;
class UObsidianItemSlot;
class UObsidianInventoryItemDefinition;
class UObsidianInventorySlot;
class UObsidianInventoryItemInstance;
class UGridPanel;
class USizeBox;
class UOverlay;
class UGridSlot;
class UObsidianInventoryWidgetController;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHoverOverSlotSignature, const UObsidianItemSlot* AffectedSlot, const bool bEntered);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMouseButtonDownOnSlotSignature, const UObsidianItemSlot* AffectedSlot, const bool bShiftDown);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianInventory : public UObsidianMainOverlayWidgetBase
{
	GENERATED_BODY()

public:
	UObsidianInventory();

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void HandleWidgetControllerSet() override;
	
	float GetInventoryWidth() const
	{
		return RootSizeBoxWidth;
	}

	float GetInventoryHeight() const
	{
		return RootSizeBoxHeight;
	}

public:
	FOnHoverOverSlotSignature OnHoverOverSlotDelegate;
	FOnMouseButtonDownOnSlotSignature OnMouseButtonDownOnSlotDelegate;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UGridPanel> Slots_GridPanel;

private:
	/** Function that triggers when automatically adding item. E.g. from the ground when inventory is hidden. */
	void OnItemAdded(const FObsidianItemVisuals& ItemVisuals);
	void OnItemChanged(const FObsidianItemVisuals& ItemVisuals);

	void SetupGrid();

	void OnItemLeftMouseButtonPressed(const FVector2D& ItemDesiredPosition, UObsidianItem* ItemWidget, const bool bShiftDown);
	void OnItemMouseEntered(const FVector2D& ItemDesiredPosition, UObsidianItem* ItemWidget);
	void OnItemMouseLeave(const FVector2D& ItemDesiredPosition);
	
	void OnInventorySlotHover(const UObsidianItemSlot* AffectedSlot, const bool bEntered);
	void OnInventorySlotMouseButtonDown(const UObsidianItemSlot* AffectedSlot, const bool bShiftDown);
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;
	
	UPROPERTY()
	TObjectPtr<UObsidianInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianItem> ItemWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianItemSlot> InventorySlotClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	int32 InventoryGridWidth = 12;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	int32 InventoryGridHeight = 5;

	/** Essentially, height component of inventory size. Use this instead of directly setting it on SizeBox. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	float RootSizeBoxHeight = 920.0f;

	/** Essentially, width component of inventory size. Use this instead of directly setting it on SizeBox. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	float RootSizeBoxWidth = 820.0f;
	
	int32 InventoryGridSize;
	TMap<FVector2D, UObsidianItemSlot*> InventoryLocationToSlotMap;

	/** Array of slots that are affected by item hover, to clear it later. */
	TArray<UObsidianItemSlot*> AffectedInventorySlots;
};

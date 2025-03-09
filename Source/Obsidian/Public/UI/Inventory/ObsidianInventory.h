// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/ObsidianMainOverlayWidgetBase.h"
#include "ObsidianInventory.generated.h"

struct FObsidianItemVisuals;
class UObsidianItem;
class UObsidianItemSlot;
class UObsidianInventoryItemDefinition;
class UObsidianItemSlot_Inventory;
class UObsidianItemSlot_Equipment;
class UObsidianInventoryItemInstance;
class UGridPanel;
class USizeBox;
class UOverlay;
class UGridSlot;
class UObsidianInventoryWidgetController;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHoverOverInventorySlotSignature, const UObsidianItemSlot_Inventory* AffectedSlot, const bool bEntered);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMouseButtonDownOnInventorySlotSignature, const UObsidianItemSlot_Inventory* AffectedSlot, const bool bShiftDown);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHoverOverEquipmentSlotSignature, UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseButtonDownOnEquipmentSlotSignature, const UObsidianItemSlot_Equipment* AffectedSlot);

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

	UObsidianItemSlot_Equipment* FindEquipmentSlotForTag(const FGameplayTag& Tag) const;

public:
	FOnHoverOverInventorySlotSignature OnHoverOverInventorySlotDelegate;
	FOnMouseButtonDownOnInventorySlotSignature OnMouseButtonDownOnInventorySlotDelegate;
	
	FOnHoverOverEquipmentSlotSignature OnHoverOverEquipmentSlotDelegate;
	FOnMouseButtonDownOnEquipmentSlotSignature OnMouseButtonDownOnEquipmentSlotDelegate;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UGridPanel> Slots_GridPanel;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UGridPanel> Equipment_GridPanel;

private:
	void OnItemEquipped(const FObsidianItemVisuals& ItemVisuals);
	
	/** Function that triggers when automatically adding item. E.g. from the ground when inventory is hidden. */
	void OnItemAdded(const FObsidianItemVisuals& ItemVisuals);
	void OnItemChanged(const FObsidianItemVisuals& ItemVisuals);

	void SetupInventoryGrid();
	void SetupEquipmentSlots();

	void OnItemLeftMouseButtonPressed(const FVector2D& ItemDesiredPosition, UObsidianItem* ItemWidget, const bool bShiftDown);
	void OnItemMouseEntered(const FVector2D& ItemDesiredPosition, UObsidianItem* ItemWidget);
	void OnItemMouseLeave(const FVector2D& ItemDesiredPosition);
	
	void OnInventorySlotHover(const UObsidianItemSlot_Inventory* AffectedSlot, const bool bEntered);
	void OnInventorySlotMouseButtonDown(const UObsidianItemSlot_Inventory* AffectedSlot, const bool bShiftDown);

	void OnEquipmentSlotHover(UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered);
	void OnEquipmentSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot);
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;
	
	UPROPERTY()
	TObjectPtr<UObsidianInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianItem> ItemWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianItemSlot_Inventory> InventorySlotClass;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianItemSlot_Equipment> RightHand_EquipmentSlot;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianItemSlot_Equipment> LeftHand_EquipmentSlot;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianItemSlot_Equipment> Helmet_EquipmentSlot;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianItemSlot_Equipment> BodyArmor_EquipmentSlot;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianItemSlot_Equipment> Belt_EquipmentSlot;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianItemSlot_Equipment> Gloves_EquipmentSlot;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianItemSlot_Equipment> Boots_EquipmentSlot;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianItemSlot_Equipment> Amulet_EquipmentSlot;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianItemSlot_Equipment> RightRing_EquipmentSlot;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianItemSlot_Equipment> LeftRing_EquipmentSlot;
	
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
	
	UPROPERTY()
	TMap<FVector2D, UObsidianItemSlot_Inventory*> InventoryLocationToSlotMap;
	
	/** Array of slots that are affected by item hover, to clear it later. */
	UPROPERTY()
	TArray<UObsidianItemSlot_Inventory*> AffectedInventorySlots;

	UPROPERTY()
	TArray<UObsidianItemSlot_Equipment*> EquipmentSlots;
};

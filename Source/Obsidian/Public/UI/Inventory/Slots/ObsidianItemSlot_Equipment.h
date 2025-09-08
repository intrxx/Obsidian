// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project


#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot.h"
#include "ObsidianItemSlot_Equipment.generated.h"

class UObsidianEquipmentPanel;
class UObsidianSlotBlockadeItem;
class UObsidianItem;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquipmentSlotHoverSignature, UObsidianItemSlot_Equipment* HoveredSlot, const bool bEntered);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquipmentSlotPressedSignature, const UObsidianItemSlot_Equipment* HoveredSlot, const bool bShiftDown);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemSlot_Equipment : public UObsidianItemSlot
{
	GENERATED_BODY()

public:
	void InitializeSlot(const FGameplayTag& InSlotTag, const FGameplayTag& InSisterSlotTag = FGameplayTag::EmptyTag);

	FGameplayTag GetSlotTag() const
	{
		return SlotTag;
	}

	FGameplayTag GetSisterSlotTag() const
	{
		return SisterSlotTag;
	}

	void SetIsBlocked(const bool bInIsBlocked)
	{
		bIsBlocked = bInIsBlocked;
	}

	void AddItemToSlot(UObsidianItem* InItemWidget, const float ItemSlotPadding = 0.0f);
	void AddItemToSlot(UObsidianSlotBlockadeItem* InItemWidget, const float ItemSlotPadding = 0.0f);

public:
	FOnEquipmentSlotHoverSignature OnEquipmentSlotHoverDelegate;
	FOnEquipmentSlotPressedSignature OnEquipmentSlotPressedDelegate;
	
protected:
	virtual void NativePreConstruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
protected:
	UPROPERTY(EditAnywhere, meta=(Categories = "Item.Slot.Equipment,Item.SwapSlot.Equipment,Item.Slot.Functional"), Category = "Obsidian|Setup")
	FGameplayTag SlotTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, meta=(Categories = "Item.Slot.Equipment,Item.SwapSlot.Equipment,Item.Slot.Functional"), Category = "Obsidian|Setup")
	FGameplayTag SisterSlotTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SlotWidth = 128.0f;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SlotHeight = 128.0f;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SlottedItemAdditionalPadding = 0.0f;

private:
	UPROPERTY()
	TObjectPtr<UObsidianEquipmentPanel> EquipmentPanel;

	bool bIsBlocked = false;
};

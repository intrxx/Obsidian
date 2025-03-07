// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "GameplayTagContainer.h"
#include "UI/Inventory/ObsidianInventory.h"
#include "ObsidianItemSlot.generated.h"

class UObsidianInventory;
class USizeBox;
class UImage;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemSlot : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void InitializeSlot(UObsidianInventory* InOwningInventory, const FVector2D& InSlotPosition);
	void InitializeSlot(UObsidianInventory* InOwningInventory, const FGameplayTag& InSlotTag);

	FVector2D GetSlotPosition() const
	{
		return SlotPosition;
	}

	/** Sets the slot state based on bAvailable, if true sets it to green if false to red. */
	void SetSlotState(const bool bAvailable);
	void ResetSlot();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Slot_Image;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Action_Image;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	FSlateBrush SlotGreenLightColor;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	FSlateBrush SlotRedLightColor;

	UPROPERTY()
	TObjectPtr<UObsidianInventory> OwningInventory;
	
	FVector2D SlotPosition;
	FGameplayTag SlotTag;
};


// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/Inventory/SubWidgets/ObsidianItemSlot.h"
#include "GameplayTagContainer.h"
#include "ObsidianItemSlot_Equipment.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemSlot_Equipment : public UObsidianItemSlot
{
	GENERATED_BODY()

public:
	void InitializeSlot(UObsidianInventory* InOwningInventory, const FGameplayTag& InSlotTag);
	void InitializeSlot(UObsidianInventory* InOwningInventory);

	FGameplayTag GetSlotTag() const
	{
		return SlotTag;
	}
	
protected:
	virtual void NativePreConstruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
protected:
	UPROPERTY(EditAnywhere, meta=(Categories = "Equipment.Slot"), Category = "Obsidian|Setup")
	FGameplayTag SlotTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SlotWidth = 128.0f;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SlotHeight = 128.0f;
};

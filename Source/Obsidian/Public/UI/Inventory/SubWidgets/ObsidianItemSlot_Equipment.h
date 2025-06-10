// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project


#include "UI/Inventory/SubWidgets/ObsidianItemSlot.h"
#include "ObsidianItemSlot_Equipment.generated.h"

class UObsidianEquipmentPanel;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemSlot_Equipment : public UObsidianItemSlot
{
	GENERATED_BODY()

public:
	void InitializeSlot(UObsidianEquipmentPanel* InEquipmentPanel, const FGameplayTag& InSlotTag);
	void InitializeSlot(UObsidianEquipmentPanel* InEquipmentPanel);

	FGameplayTag GetSlotTag() const
	{
		return SlotTag;
	}

	FGameplayTag GetSisterSlotTag() const
	{
		return SisterSlotTag;
	}
	
protected:
	virtual void NativePreConstruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
protected:
	UPROPERTY(EditAnywhere, meta=(Categories = "Equipment"), Category = "Obsidian|Setup")
	FGameplayTag SlotTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, meta=(Categories = "Equipment"), Category = "Obsidian|Setup")
	FGameplayTag SisterSlotTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SlotWidth = 128.0f;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SlotHeight = 128.0f;

	UPROPERTY()
	TObjectPtr<UObsidianEquipmentPanel> EquipmentPanel;
};

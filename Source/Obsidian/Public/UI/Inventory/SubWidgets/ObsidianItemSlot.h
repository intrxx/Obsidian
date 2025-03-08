// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
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
	/** Sets the slot state based on bAvailable, if true sets it to green if false to red. */
	void SetSlotState(const bool bAvailable);
	void ResetSlot();

protected:
	virtual void NativeConstruct() override;

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
};


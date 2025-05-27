// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "UI/Inventory/ObsidianInventory.h"

#include "UI/ObsidianWidgetBase.h"
#include "ObsidianItemSlot.generated.h"

class UObsidianInventory;
class USizeBox;
class UImage;

/**
 * 
 */
UENUM()
enum EObsidianItemSlotState
{
	ISS_Neutral,
	ISS_GreenLight,
	ISS_RedLight,
	ISS_Blocked
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemSlot : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	/** Sets the slot state, switches the slot highlight based on provided state. */
	void SetSlotState(EObsidianItemSlotState InState) const;

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

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	FSlateBrush SlotBlockedLightColor;

	UPROPERTY()
	TObjectPtr<UObsidianInventory> OwningInventory;
};


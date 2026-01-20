// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "UI/Inventory/ObsidianInventory.h"

#include "UI/ObsidianWidgetBase.h"
#include "ObsidianSlotBase.generated.h"

class UObsidianGridPanel;
class UObsidianInventory;
class USizeBox;
class UImage;

/**
 * Enum which describes different states that the Slot will switch to when interacting with it.
 */
UENUM()
enum class EObsidianItemSlotState : uint8
{
	/** Normal color of the slot (the action image is hidden). */
	Neutral = 0,

	/** The Player is able to place or take the item he/she is hovering over. */
	GreenLight,

	/** The slot is being selected without an item in hands. */
	Selected,

	/** The Player is NOT able to place or take the item he/she is hovering over. */
	RedLight,

	/** The slot is blocked e.g. Two-Handed item is equipped in the other slot. */
	Blocked
};

UENUM()
enum class EObsidianItemSlotStatePriority : uint8
{
	/** Used for hover logic. */
	Low = 0,

	/** Used for tooltip logic (highlighting possible slots for placing dragged item). */
	High,

	/** Always takes priority, used for blocked slots. */
	TakePriority
};

/**
 * Base class of different slots which can represent item holding panels.
 */
UCLASS()
class OBSIDIAN_API UObsidianSlotBase : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	/** Sets the slot state, switches the slot highlight based on provided state. */
	void SetSlotState(const EObsidianItemSlotState InState, const EObsidianItemSlotStatePriority InPriority);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Main_Overlay;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Slot_Image;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Action_Image;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	FSlateBrush SlotGreenLightColor;
	
	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	FSlateBrush SlotRedLightColor;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	FSlateBrush SlotBlockedLightColor;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	FSlateBrush SlotSelectedLightColor;

private:
	UPROPERTY()
	EObsidianItemSlotStatePriority CurrentStatePriority = EObsidianItemSlotStatePriority::Low;
};


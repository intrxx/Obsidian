// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "OInventoryItemFragment_Usable.generated.h"

class UObsidianUsableShard;

UENUM(BlueprintType)
enum class EObsidianUsableItemType : uint8
{
	UIT_None = 0 UMETA(DisplayName = "None"),
	
	/** Crafting items are used onto other items to cause some effect e.g. Scroll of Identification will identify an unidentified item. */
	UIT_Crafting UMETA(DisplayName = "Crafting"),

	/** Activation items will cause some effect to happen when clicking them e.g. Opening Portal to town after clicking Scroll of Teleportation. */
	UIT_Activation UMETA(DisplayName = "Activation")
};

/**
 * Fragment that allows the item to have Usable functionality.
 */
UCLASS(DisplayName="Usable")
class OBSIDIAN_API UOInventoryItemFragment_Usable : public UObsidianInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	//~ Start of UObsidianInventoryItemFragment
	virtual void OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const override;
	//~ End of UObsidianInventoryItemFragment

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Usable")
	EObsidianUsableItemType UsableItemType = EObsidianUsableItemType::UIT_None;
	
	UPROPERTY(EditDefaultsOnly, Category = "Usable")
	TObjectPtr<UObsidianUsableShard> UsableShard = nullptr;
};

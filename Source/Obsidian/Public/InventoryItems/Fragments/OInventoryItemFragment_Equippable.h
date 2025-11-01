// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>
#include <GameplayTagContainer.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "OInventoryItemFragment_Equippable.generated.h"

class UObsidianAbilitySet;
class AObsidianSpawnedEquipmentPiece;

USTRUCT(BlueprintType)
struct FObsidianEquipmentActor
{
	GENERATED_BODY()

public:
	FObsidianEquipmentActor(){}

	void OverrideAttachSocket(const FGameplayTag& SlotTag);
	
public:
	UPROPERTY(EditAnywhere, Category = "Obsidian|Equipment")
	TSubclassOf<AObsidianSpawnedEquipmentPiece> ActorToSpawn = nullptr;

	/** Whether to override Attach Socket based on the slot that the item is being equipped in, defaulting to true since it will be more frequent. */
	UPROPERTY(EditAnywhere, Category = "Obsidian|Equipment")
	bool bOverrideAttachSocket = true;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "!bOverrideAttachSocket"), Category = "Obsidian|Equipment")
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Equipment")
	FTransform AttachTransform;
};

/**
 * Fragment allowing the item to be equipped.
 */
UCLASS(DisplayName="Equippable")
class OBSIDIAN_API UOInventoryItemFragment_Equippable : public UObsidianInventoryItemFragment
{
	GENERATED_BODY()

public:
	//~ Start of UObsidianInventoryItemFragment
	virtual void OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const override;
	//~ End of UObsidianInventoryItemFragment
	
	FObsidianItemRequirements GetItemDefaultEquippingRequirements() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipping")
	TArray<FObsidianEquipmentActor> ActorsToSpawn;
	
	/** Should this item block the other equipment slot, e.g. Two-Handed Weapons. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipping")
	bool bShouldBlockOtherSlot = false;

	UPROPERTY(EditDefaultsOnly, Category = "Equipping")
	FObsidianItemRequirements DefaultEquippingRequirements = FObsidianItemRequirements();
};

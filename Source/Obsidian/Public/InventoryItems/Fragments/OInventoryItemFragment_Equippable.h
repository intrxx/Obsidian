// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "OInventoryItemFragment_Equippable.generated.h"

struct FObsidianSavedEquipmentPiece;

class UObsidianAbilitySet;
class AObsidianSpawnedEquipmentPiece;

USTRUCT(BlueprintType)
struct FObsidianEquipmentActor
{
	GENERATED_BODY()

public:
	FObsidianEquipmentActor(){}
	FObsidianEquipmentActor(const FObsidianSavedEquipmentPiece& InSavedEquipmentActor);

	void OverrideAttachSocket(const FGameplayTag& InSlotTag);

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
UCLASS(DisplayName="Equippable", meta = (PrioritizeCategories = "Equipping"))
class OBSIDIAN_API UOInventoryItemFragment_Equippable : public UObsidianInventoryItemFragment
{
	GENERATED_BODY()

public:
	FObsidianItemRequirements GetItemDefaultEquippingRequirements() const { return DefaultEquippingRequirements; }

	//~ Begin UObsidianInventoryItemFragment Interface
	virtual void OnInstancedCreated(UObsidianInventoryItemInstance* InInstance) const override;
	//~ End UObsidianInventoryItemFragment Interface

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipping")
	TArray<FObsidianEquipmentActor> ActorsToSpawn;

	/** Should this item block the other equipment slot, e.g. Two-Handed Weapons. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipping")
	bool bShouldBlockOtherSlot = false;

	UPROPERTY(EditDefaultsOnly, Category = "Equipping")
	FObsidianItemRequirements DefaultEquippingRequirements = FObsidianItemRequirements();
};

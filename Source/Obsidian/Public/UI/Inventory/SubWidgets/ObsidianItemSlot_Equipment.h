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

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	FGameplayTag SlotTag = FGameplayTag::EmptyTag;
};

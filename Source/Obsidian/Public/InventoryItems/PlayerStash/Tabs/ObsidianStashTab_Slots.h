// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTypes/ObsidianItemTypes.h"

#include "InventoryItems/PlayerStash/ObsidianStashTab.h"
#include "ObsidianStashTab_Slots.generated.h"


/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianStashTab_Slots : public UObsidianStashTab
{
	GENERATED_BODY()

public:
	UObsidianStashTab_Slots(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void MarkSpaceInTab(UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition) override;
	virtual void UnmarkSpaceInTab(UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition) override;

	virtual void Construct(UObsidianPlayerStashComponent* StashComponent) override;

private:
	UPROPERTY(EditAnywhere, Category = "Obsidian|SlotsSettings")
	TArray<FObsidianSlotDefinition> TabSlots;
	
	TMap<FGameplayTag, UObsidianInventoryItemInstance*> SlotToItemMap;
};

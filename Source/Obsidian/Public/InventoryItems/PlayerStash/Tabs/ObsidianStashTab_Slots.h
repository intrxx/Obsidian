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

	virtual UObsidianInventoryItemInstance* GetInstanceAtPosition(const FObsidianItemPosition& ItemPosition) override;

	virtual bool DebugVerifyPositionFree(const FObsidianItemPosition& Position) override;

	virtual bool CanPlaceItemAtSpecificPosition(const FObsidianItemPosition& SpecifiedPosition, const FGameplayTag& ItemCategory, const FIntPoint& ItemGridSpan) override;
	virtual bool FindFirstAvailablePositionForItem(FObsidianItemPosition& OutFirstAvailablePosition, const FGameplayTag& ItemCategory, const FIntPoint& ItemGridSpan) override;

	virtual void Construct(UObsidianPlayerStashComponent* StashComponent) override;
	virtual void MarkSpaceInTab(UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition) override;
	virtual void UnmarkSpaceInTab(UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition) override;

protected:
	FObsidianSlotDefinition FindSlotByTag(const FGameplayTag& SlotTag) const;
	
private:

#if WITH_GAMEPLAY_DEBUGGER
	friend class FGameplayDebuggerCategory_PlayerStash;
#endif
	
	UPROPERTY(EditAnywhere, Category = "Obsidian|SlotsSettings")
	TArray<FObsidianSlotDefinition> TabSlots;

	UPROPERTY()
	TMap<FGameplayTag, UObsidianInventoryItemInstance*> SlotToItemMap;
};

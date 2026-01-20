// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "InventoryItems/PlayerStash/ObsidianStashItemList.h"

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
	TArray<FObsidianStashSlotDefinition> GetSlots() const;

	virtual bool DebugVerifyPositionFree(const FObsidianItemPosition& Position) override;

	virtual bool CanPlaceItemAtSpecificPosition(const FObsidianItemPosition& SpecifiedPosition, const FGameplayTag& ItemCategory, const FGameplayTag& ItemBaseType, const FIntPoint& ItemGridSpan) override;
	virtual bool FindFirstAvailablePositionForItem(FObsidianItemPosition& OutFirstAvailablePosition, const FGameplayTag& ItemCategory, const FGameplayTag& ItemBaseType, const FIntPoint& ItemGridSpan) override;

	virtual bool CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& SpecifiedPosition, const UObsidianInventoryItemInstance* ReplacingInstance) override;
	virtual bool CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& SpecifiedPosition, const TSubclassOf<UObsidianInventoryItemDefinition>& ReplacingDef) override;
	
	virtual void Construct(UObsidianPlayerStashComponent* StashComponent) override;
	virtual void MarkSpaceInTab(UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition) override;
	virtual void UnmarkSpaceInTab(UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition) override;

protected:
	FObsidianStashSlotDefinition FindSlotByTag(const FGameplayTag& SlotTag) const;

private:
	bool CheckReplacementPossible(const FObsidianItemPosition& SpecifiedPosition, const FGameplayTag& ReplacingItemCategory, const FGameplayTag& ReplacingItemBaseType) const;
	
private:

#if WITH_GAMEPLAY_DEBUGGER
	friend class FGameplayDebuggerCategory_PlayerStash;
#endif
	
	UPROPERTY(EditAnywhere, Category = "Obsidian|SlotsSettings")
	TArray<FObsidianStashSlotDefinition> TabSlots;

	UPROPERTY()
	TMap<FGameplayTag, UObsidianInventoryItemInstance*> SlotToItemMap;
};

// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "InventoryItems/PlayerStash/ObsidianStashItemList.h"
#include "InventoryItems/PlayerStash/ObsidianStashTab.h"

#include "ObsidianStashTab_Slots.generated.h"

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API UObsidianStashTab_Slots : public UObsidianStashTab
{
	GENERATED_BODY()

public:
	UObsidianStashTab_Slots(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	TArray<FObsidianStashSlotDefinition> GetSlots() const { return TabSlots; }

	//~ Begin UObsidianStashTab Interface
	virtual UObsidianInventoryItemInstance* GetInstanceAtPosition(const FObsidianItemPosition& InItemPosition) override;

	virtual bool DebugVerifyPositionFree(const FObsidianItemPosition& InPosition) override;

	virtual bool CanPlaceItemAtSpecificPosition(const FObsidianItemPosition& InSpecifiedPosition, const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseType, const FIntPoint& InItemGridSpan) override;
	virtual bool FindFirstAvailablePositionForItem(FObsidianItemPosition& OutFirstAvailablePosition, const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseType, const FIntPoint& InItemGridSpan) override;

	virtual bool CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& InSpecifiedPosition, const UObsidianInventoryItemInstance* InReplacingInstance) override;
	virtual bool CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& InSpecifiedPosition, const TSubclassOf<UObsidianInventoryItemDefinition>& InReplacingDef) override;

	virtual void Construct(UObsidianPlayerStashComponent* InStashComponent) override;
	virtual void MarkSpaceInTab(UObsidianInventoryItemInstance* InItemInstance, const FObsidianItemPosition& InAtPosition) override;
	virtual void UnmarkSpaceInTab(UObsidianInventoryItemInstance* InItemInstance, const FObsidianItemPosition& InAtPosition) override;
	//~ End UObsidianStashTab Interface

protected:
	FObsidianStashSlotDefinition FindSlotByTag(const FGameplayTag& InSlotTag) const;

private:
#if WITH_GAMEPLAY_DEBUGGER
	friend class FGameplayDebuggerCategory_PlayerStash;
#endif

	bool CheckReplacementPossible(const FObsidianItemPosition& InSpecifiedPosition, const FGameplayTag& InReplacingItemCategory, const FGameplayTag& InReplacingItemBaseType) const;

	UPROPERTY(EditAnywhere, Category = "Obsidian|SlotsSettings")
	TArray<FObsidianStashSlotDefinition> TabSlots;

	UPROPERTY()
	TMap<FGameplayTag, UObsidianInventoryItemInstance*> SlotToItemMap;
};

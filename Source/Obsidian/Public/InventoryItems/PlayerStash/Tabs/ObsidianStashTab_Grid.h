// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "InventoryItems/PlayerStash/ObsidianStashTab.h"
#include "ObsidianStashTab_Grid.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianStashTab_Grid : public UObsidianStashTab
{
	GENERATED_BODY()

public:
	UObsidianStashTab_Grid(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual UObsidianInventoryItemInstance* GetInstanceAtPosition(const FObsidianItemPosition& ItemPosition) override;

	virtual bool CanPlaceItemAtSpecificPosition(const FObsidianItemPosition& SpecifiedPosition, const FGameplayTag& ItemCategory, const FIntPoint& ItemGridSpan) override;
	virtual bool FindFirstAvailablePositionForItem(FObsidianItemPosition& OutFirstAvailablePosition, const FGameplayTag& ItemCategory, const FIntPoint& ItemGridSpan) override;

	virtual bool DebugVerifyPositionFree(const FObsidianItemPosition& Position) override;

	virtual void Construct(UObsidianPlayerStashComponent* StashComponent) override;
	virtual void MarkSpaceInTab(UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition) override;
	virtual void UnmarkSpaceInTab(UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition) override;
	
	int32 GetGridWidth() const;
	int32 GetGridHeight() const;

private:

#if WITH_GAMEPLAY_DEBUGGER
	friend class FGameplayDebuggerCategory_PlayerStash;
#endif
	
	/**
	* Map that represents whole Grid with taken fields.
	* If a Given FIntPoint location has a true value associated with it, the field is treated as taken.
	*/
	TMap<FIntPoint, bool> GridStateMap;

	UPROPERTY()
	TMap<FIntPoint, UObsidianInventoryItemInstance*> GridLocationToItemMap;

	UPROPERTY(EditAnywhere, Category = "Obsidian|GridSettings")
	int32 GridWidth = 12;
	
	UPROPERTY(EditAnywhere, Category = "Obsidian|GridSettings")
	int32 GridHeight = 10;
};

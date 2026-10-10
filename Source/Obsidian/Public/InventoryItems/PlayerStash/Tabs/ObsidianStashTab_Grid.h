// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

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
	UObsidianStashTab_Grid(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	virtual UObsidianInventoryItemInstance* GetInstanceAtPosition(const FObsidianItemPosition& InItemPosition) override;

	virtual bool CanPlaceItemAtSpecificPosition(const FObsidianItemPosition& InSpecifiedPosition, const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseType, const FIntPoint& InItemGridSpan) override;
	virtual bool FindFirstAvailablePositionForItem(FObsidianItemPosition& OutFirstAvailablePosition, const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseType, const FIntPoint& InItemGridSpan) override;
	
	virtual bool CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& InSpecifiedPosition, const UObsidianInventoryItemInstance* InReplacingInstance) override;
	virtual bool CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& InSpecifiedPosition, const TSubclassOf<UObsidianInventoryItemDefinition>& InReplacingDef) override;
	
	virtual bool DebugVerifyPositionFree(const FObsidianItemPosition& InPosition) override;

	virtual void Construct(UObsidianPlayerStashComponent* InStashComponent) override;
	virtual void MarkSpaceInTab(UObsidianInventoryItemInstance* InItemInstance, const FObsidianItemPosition& InAtPosition) override;
	virtual void UnmarkSpaceInTab(UObsidianInventoryItemInstance* InItemInstance, const FObsidianItemPosition& InAtPosition) override;
	
	int32 GetGridWidth() const;
	int32 GetGridHeight() const;

private:
	bool CheckReplacementPossible(const FObsidianItemPosition& InSpecifiedPosition, const FIntPoint& InReplacingItemGridSpan) const;

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

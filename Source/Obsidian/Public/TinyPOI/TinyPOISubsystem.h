// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "TinyPOI/TinyPOIBuilder.h"
#include "TinyPOI/TinyPOITypes.h"

#include "TinyPOISubsystem.generated.h"

class ATinyPOIData;

/** An ATinyPOIData registered in the UTinyPOISubsystem. */
USTRUCT()
struct FTinyPOIRegisteredData
{
	GENERATED_BODY()

	void Reset(const int32 InGeneration)
	{
		POIData = nullptr;
		Generation = InGeneration;
	}

	UPROPERTY()
	TObjectPtr<ATinyPOIData> POIData = nullptr;

	/** Starts at 1, as 0 is the Generation of an invalid handle. */
	int32 Generation = 1;
};

/**
 * Knows every ATinyPOIData that is loaded in the world and answers queries about the baked Points Of Interest.
 *
 * In the editor world it also owns the FTinyPOIBuilder and decides when it runs: on Tick, whenever the markup has
 * changed. A bake can be forced with the "TinyPOI.Rebuild" console command, "TinyPOI.DebugDraw 1" draws the baked data.
 */
UCLASS()
class OBSIDIAN_API UTinyPOISubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Registration of the baked data, called by ATinyPOIData.
	 */

	FTinyPOIDataHandle RegisterPOIData(ATinyPOIData& InPOIData);
	void UnregisterPOIData(ATinyPOIData& InPOIData);

	TConstArrayView<FTinyPOIRegisteredData> GetRegisteredPOIData() const { return RegisteredPOIData; }

	/**
	 * Queries, they go through every registered Data.
	 */

	/** @return The Storage registered with the handle, nullptr if the Data is no longer loaded. */
	const FTinyPOIStorage* GetStorage(const FTinyPOIDataHandle InDataHandle) const;

	/** @return The Point of the handle, nullptr if its Data is no longer loaded. Do not hold onto the pointer. */
	const FTinyPOIPoint* GetPoint(const FTinyPOIHandle InHandle) const;

	/** @return Number of Points in all the registered Data. */
	int32 GetNumPoints() const;

	/**
	 * Finds the Point closest to the location.
	 * @param InLocation Location to search around.
	 * @param InMaxDistance Points further than this are ignored.
	 * @param InTagFilter Only Points with this Tag or one of its children are considered, invalid Tag accepts every Point.
	 * @param OutHandle Handle of the closest Point.
	 * @return True if a Point was found.
	 */
	bool FindNearestPoint(const FVector& InLocation, const float InMaxDistance, const FGameplayTag& InTagFilter,
		FTinyPOIHandle& OutHandle) const;

	/**
	 * Finds every Point in range of the location, see FindNearestPoint for the parameters.
	 * @return True if any Point was found.
	 */
	bool FindPointsInRange(const FVector& InLocation, const float InRange, const FGameplayTag& InTagFilter,
		TArray<FTinyPOIHandle>& OutHandles) const;

#if WITH_EDITOR
	/**
	 * Baking, editor world only.
	 */

	FTinyPOIBuilder& GetBuilder() { return Builder; }

	/**
	 * Makes sure every level with markup has an ATinyPOIData and bakes the ones that are out of date. Does nothing in
	 * a game world, there the data is used as it was saved.
	 * @param bInForceRebuild Bakes all the Data, even if the markup did not change.
	 */
	void RebuildData(const bool bInForceRebuild = false);
#endif

	//~ Begin FTickableGameObject Interface
	virtual void Tick(float InDeltaTime) override;
	virtual TStatId GetStatId() const override;
#if WITH_EDITOR
	virtual bool IsTickableInEditor() const override { return true; }
#endif
	//~ End FTickableGameObject Interface

protected:
	/** Registers the Data that was loaded before the Subsystem existed. */
	void RegisterPOIDataInstances();

#if WITH_EDITOR
	/** Spawns an ATinyPOIData in every level that has markup, but no Data to bake it into. */
	void SpawnMissingPOIData();
#endif

#if ENABLE_DRAW_DEBUG
	void DebugDraw() const;
#endif

	//~ Begin UWorldSubsystem Interface
	virtual void PostInitialize() override;
	//~ End UWorldSubsystem Interface

private:
	/** Handles index into this array, so an entry is never removed, only freed for the next Data to reuse. */
	UPROPERTY()
	TArray<FTinyPOIRegisteredData> RegisteredPOIData;

	TArray<int32> RegisteredPOIDataFreeList;

#if WITH_EDITOR
	FTinyPOIBuilder Builder;
#endif
};

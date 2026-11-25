// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "TinyPOITypes.generated.h"

/**
 * TinyPOI (Tiny Points Of Interest) is a small, self-contained example of how the ZoneGraph plugin bakes level markup
 * into a data struct. Every piece has a counterpart in the plugin:
 *
 *   UTinyPOIComponent, ATinyPOIMarker   (UZoneShapeComponent, AZoneShape)   editor only markup placed in the level
 *   FTinyPOIBuilder                     (FZoneGraphBuilder)                 editor only, turns the markup into the baked data
 *   FTinyPOIStorage                     (FZoneGraphStorage)                 the baked data, flat arrays that are cheap to query
 *   ATinyPOIData                        (AZoneGraphData)                    one per level, saves the baked data with the level
 *   UTinyPOISubsystem                   (UZoneGraphSubsystem)               registry of the loaded data, runs the bake, queries
 *
 * In the editor: markup registers itself in the Builder of the Subsystem and marks it dirty whenever it changes. The
 * Subsystem ticks in the editor, spawns an ATinyPOIData in every level that has markup and lets the Builder fill its
 * Storage, but only when the hash of the markup differs from the hash the Storage was baked with.
 *
 * In the game: the markup is stripped, only ATinyPOIData is loaded. It registers its Storage in the Subsystem, which
 * hands out handles and answers queries.
 */

/** Handle of a registered ATinyPOIData, valid for as long as the Data stays registered in the UTinyPOISubsystem. */
USTRUCT()
struct FTinyPOIDataHandle
{
	GENERATED_BODY()

	FTinyPOIDataHandle() = default;
	FTinyPOIDataHandle(const uint16 InIndex, const uint16 InGeneration)
		: Index(InIndex)
		, Generation(InGeneration)
	{}

	static constexpr uint16 InvalidGeneration = 0;

	bool operator==(const FTinyPOIDataHandle& InOther) const
	{
		return Index == InOther.Index && Generation == InOther.Generation;
	}

	void Reset()
	{
		Index = 0;
		Generation = InvalidGeneration;
	}

	/** Any Index is valid, it is the Generation that tells if the handle was ever given out. */
	bool IsValid() const { return Generation != InvalidGeneration; }

	/** Index of the Data in the Subsystem. */
	UPROPERTY(Transient)
	uint16 Index = 0;

	/** Bumped every time the Index is reused, so handles to Data that is no longer loaded can be told apart. */
	UPROPERTY(Transient)
	uint16 Generation = InvalidGeneration;
};

/** Handle of a single baked Point, this is what queries return. */
USTRUCT()
struct FTinyPOIHandle
{
	GENERATED_BODY()

	FTinyPOIHandle() = default;
	FTinyPOIHandle(const int32 InIndex, const FTinyPOIDataHandle InDataHandle)
		: Index(InIndex)
		, DataHandle(InDataHandle)
	{}

	bool operator==(const FTinyPOIHandle& InOther) const
	{
		return Index == InOther.Index && DataHandle == InOther.DataHandle;
	}

	void Reset()
	{
		Index = INDEX_NONE;
		DataHandle.Reset();
	}

	bool IsValid() const { return Index != INDEX_NONE && DataHandle.IsValid(); }

	/** Index into FTinyPOIStorage::Points. */
	UPROPERTY(Transient)
	int32 Index = INDEX_NONE;

	/** The Data that holds the Point. */
	UPROPERTY(Transient)
	FTinyPOIDataHandle DataHandle;
};

/** A single baked Point Of Interest, everything is in world space. */
USTRUCT()
struct FTinyPOIPoint
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Position = FVector::ZeroVector;

	UPROPERTY()
	FVector Forward = FVector::ForwardVector;

	UPROPERTY()
	float Radius = 0.0f;

	UPROPERTY()
	FGameplayTag Tag;
};

/** Range of FTinyPOIStorage::Points that share the same Tag. */
USTRUCT()
struct FTinyPOITagRange
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 PointsBegin = 0;

	/** One past the last Point of the range. */
	UPROPERTY()
	int32 PointsEnd = 0;
};

/**
 * The baked data of one level. It is only ever written by FTinyPOIBuilder in the editor, the game reads it as it was
 * saved with the level.
 */
USTRUCT()
struct OBSIDIAN_API FTinyPOIStorage
{
	GENERATED_BODY()

	/** Clears the baked data, the DataHandle stays as it describes the registration and not the data. */
	void Reset();

	/**
	 * Finds the Point closest to the location.
	 * @param InLocation Location to search around.
	 * @param InMaxDistance Points further than this are ignored.
	 * @param InTagFilter Only Points with this Tag or one of its children are considered, invalid Tag accepts every Point.
	 * @param OutHandle Handle of the closest Point.
	 * @param OutDistanceSqr Squared distance to the closest Point.
	 * @return True if a Point was found.
	 */
	bool FindNearestPoint(const FVector& InLocation, const float InMaxDistance, const FGameplayTag& InTagFilter,
		FTinyPOIHandle& OutHandle, float& OutDistanceSqr) const;

	/**
	 * Appends every Point in range of the location to the array, see FindNearestPoint for the parameters.
	 * @return True if any Point was added.
	 */
	bool FindPointsInRange(const FVector& InLocation, const float InRange, const FGameplayTag& InTagFilter,
		TArray<FTinyPOIHandle>& OutHandles) const;

	/** All the Points, sorted by Tag so every Tag is one contiguous range. */
	UPROPERTY()
	TArray<FTinyPOIPoint> Points;

	/** A range of Points for every Tag used in the level, this is what makes a query by Tag skip all the other Points. */
	UPROPERTY()
	TArray<FTinyPOITagRange> TagRanges;

	/** Bounding box of all the Points with their radii, lets a query reject the whole Storage at once. */
	UPROPERTY()
	FBox Bounds = FBox(ForceInit);

	/** The handle this Storage is registered with in the UTinyPOISubsystem, used for the query results. Not saved. */
	FTinyPOIDataHandle DataHandle;
};

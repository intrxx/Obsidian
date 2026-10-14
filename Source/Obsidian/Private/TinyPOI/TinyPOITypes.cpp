// Copyright 2026 out of sCope team - intrxx

#include "TinyPOI/TinyPOITypes.h"


namespace TinyPOIQuery
{
	/** Calls the function with the index of every Point in range of the location that passes the Tag filter. */
	template <typename FunctionType>
	void ForEachPointInRange(const FTinyPOIStorage& InStorage, const FVector& InLocation, const float InRange,
		const FGameplayTag& InTagFilter, FunctionType&& InFunction)
	{
		// The Storage has no spatial structure as it is meant to stay tiny, this is where ZoneGraph queries its BV-Tree.
		const double RangeSqr = FMath::Square(static_cast<double>(InRange));
		if(InStorage.Bounds.IsValid == false || InStorage.Bounds.ComputeSquaredDistanceToPoint(InLocation) > RangeSqr)
		{
			return;
		}

		for(const FTinyPOITagRange& TagRange : InStorage.TagRanges)
		{
			if(InTagFilter.IsValid() && TagRange.Tag.MatchesTag(InTagFilter) == false)
			{
				continue;
			}

			for(int32 PointIndex = TagRange.PointsBegin; PointIndex < TagRange.PointsEnd; ++PointIndex)
			{
				const double DistanceSqr = FVector::DistSquared(InLocation, InStorage.Points[PointIndex].Position);
				if(DistanceSqr <= RangeSqr)
				{
					InFunction(PointIndex, DistanceSqr);
				}
			}
		}
	}
}

void FTinyPOIStorage::Reset()
{
	Points.Reset();
	TagRanges.Reset();
	Bounds.Init();
}

bool FTinyPOIStorage::FindNearestPoint(const FVector& InLocation, const float InMaxDistance, const FGameplayTag& InTagFilter,
	FTinyPOIHandle& OutHandle, float& OutDistanceSqr) const
{
	int32 NearestIndex = INDEX_NONE;
	double NearestDistanceSqr = TNumericLimits<double>::Max();

	TinyPOIQuery::ForEachPointInRange(*this, InLocation, InMaxDistance, InTagFilter,
		[&NearestIndex, &NearestDistanceSqr](const int32 InPointIndex, const double InDistanceSqr)
		{
			if(InDistanceSqr < NearestDistanceSqr)
			{
				NearestIndex = InPointIndex;
				NearestDistanceSqr = InDistanceSqr;
			}
		});

	if(NearestIndex == INDEX_NONE)
	{
		return false;
	}

	OutHandle = FTinyPOIHandle(NearestIndex, DataHandle);
	OutDistanceSqr = static_cast<float>(NearestDistanceSqr);
	return true;
}

bool FTinyPOIStorage::FindPointsInRange(const FVector& InLocation, const float InRange, const FGameplayTag& InTagFilter,
	TArray<FTinyPOIHandle>& OutHandles) const
{
	const int32 NumBefore = OutHandles.Num();

	TinyPOIQuery::ForEachPointInRange(*this, InLocation, InRange, InTagFilter,
		[this, &OutHandles](const int32 InPointIndex, const double InDistanceSqr)
		{
			OutHandles.Emplace(InPointIndex, DataHandle);
		});

	return OutHandles.Num() > NumBefore;
}

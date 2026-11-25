// Copyright 2026 out of sCope team - intrxx

#include "TinyPOIBuilder.h"

#if WITH_EDITOR

#include "Engine/Level.h"

#include "TinyPOIComponent.h"
#include "TinyPOIData.h"
#include "TinyPOILogCategories.h"
#include "TinyPOITypes.h"


void FTinyPOIBuilder::RegisterPOIComponent(UTinyPOIComponent& InComponent)
{
	Components.AddUnique(&InComponent);
	bIsDirty = true;
}

void FTinyPOIBuilder::UnregisterPOIComponent(UTinyPOIComponent& InComponent)
{
	Components.RemoveSingleSwap(&InComponent);
	bIsDirty = true;
}

void FTinyPOIBuilder::OnPOIComponentChanged(UTinyPOIComponent& InComponent)
{
	// Nothing is baked here, changes come in bursts (e.g. every frame of dragging a Marker) and are picked up by the
	// hash check of the next BuildAll().
	bIsDirty = true;
}

void FTinyPOIBuilder::BuildAll(const TConstArrayView<ATinyPOIData*> InAllPOIData, const bool bInForceRebuild)
{
	for(ATinyPOIData* POIData : InAllPOIData)
	{
		if(POIData == nullptr)
		{
			continue;
		}

		const ULevel* Level = POIData->GetLevel();
		const uint32 CombinedHash = CalculateCombinedHash(Level);
		if(bInForceRebuild == false && POIData->GetCombinedHash() == CombinedHash)
		{
			UE_LOG(LogTinyPOI, Verbose, TEXT("[%s] is up to date, the bake is skipped."), *POIData->GetPathName());
			continue;
		}

		// Marks the level as dirty and makes the bake a part of the transaction that caused it, if there is one.
		POIData->Modify();

		FTinyPOIStorage& Storage = POIData->GetStorageMutable();
		BuildStorage(Level, Storage);
		POIData->SetCombinedHash(CombinedHash);

		UE_LOG(LogTinyPOI, Log, TEXT("Baked [%d] Points Of Interest with [%d] Tags into [%s]."), Storage.Points.Num(),
			Storage.TagRanges.Num(), *POIData->GetPathName());
	}

	bIsDirty = false;
}

void FTinyPOIBuilder::BuildStorage(const ULevel* InLevel, FTinyPOIStorage& OutStorage) const
{
	OutStorage.Reset();

	// Data of the Points that have any. Until the Points are sorted, their DataIndex points in here.
	TArray<const FInstancedStruct*> SourceData;

	// Bake the markup into world space Points.
	for(const TWeakObjectPtr<UTinyPOIComponent>& WeakComponent : Components)
	{
		const UTinyPOIComponent* Component = WeakComponent.Get();
		if(Component == nullptr || Component->GetComponentLevel() != InLevel)
		{
			continue;
		}

		const FTransform& ComponentTransform = Component->GetComponentTransform();

		FTinyPOIPoint& Point = OutStorage.Points.AddDefaulted_GetRef();
		Point.PointPosition.Position = ComponentTransform.GetLocation();
		Point.PointPosition.Forward = ComponentTransform.GetUnitAxis(EAxis::X);
		Point.PointPosition.Radius = Component->GetRadius() * static_cast<float>(ComponentTransform.GetMaximumAxisScale());
		Point.Tag = Component->GetTag();

		if(Component->GetData().IsValid())
		{
			Point.DataIndex = SourceData.Add(&Component->GetData());
		}
	}

	// Sort by Tag, so every Tag ends up as one contiguous range of Points. Components register in an order that changes
	// between sessions, sorting the rest by position makes the same markup always bake into the same data.
	OutStorage.Points.Sort([](const FTinyPOIPoint& InLeft, const FTinyPOIPoint& InRight)
		{
			const int32 TagOrder = InLeft.Tag.GetTagName().Compare(InRight.Tag.GetTagName());
			if(TagOrder != 0)
			{
				return TagOrder < 0;
			}
			if(InLeft.PointPosition.Position.X != InRight.PointPosition.Position.X)
			{
				return InLeft.PointPosition.Position.X < InRight.PointPosition.Position.X;
			}
			if(InLeft.PointPosition.Position.Y != InRight.PointPosition.Position.Y)
			{
				return InLeft.PointPosition.Position.Y < InRight.PointPosition.Position.Y;
			}
			return InLeft.PointPosition.Position.Z < InRight.PointPosition.Position.Z;
		});

	// Build the data that is derived from the Points: the range of every Tag and the bounds of the whole Storage. The
	// Point data is copied in the order of the Points, which also makes it come out the same for the same markup.
	OutStorage.PointData.Reserve(SourceData.Num());
	for(int32 PointIndex = 0; PointIndex < OutStorage.Points.Num(); ++PointIndex)
	{
		FTinyPOIPoint& Point = OutStorage.Points[PointIndex];

		if(Point.HasData())
		{
			Point.DataIndex = OutStorage.PointData.Add(*SourceData[Point.DataIndex]);
		}

		if(OutStorage.TagRanges.IsEmpty() || OutStorage.TagRanges.Last().Tag != Point.Tag)
		{
			FTinyPOITagRange& TagRange = OutStorage.TagRanges.AddDefaulted_GetRef();
			TagRange.Tag = Point.Tag;
			TagRange.PointsBegin = PointIndex;
		}
		OutStorage.TagRanges.Last().PointsEnd = PointIndex + 1;

		OutStorage.Bounds += FBox::BuildAABB(Point.PointPosition.Position, FVector(Point.PointPosition.Radius));
	}
}

uint32 FTinyPOIBuilder::CalculateCombinedHash(const ULevel* InLevel) const
{
	TArray<uint32> ComponentHashes;
	for(const TWeakObjectPtr<UTinyPOIComponent>& WeakComponent : Components)
	{
		const UTinyPOIComponent* Component = WeakComponent.Get();
		if(Component && Component->GetComponentLevel() == InLevel)
		{
			ComponentHashes.Add(Component->GetPOIHash());
		}
	}

	// The combined hash is saved with the level and compared in the next session, where the components will most likely
	// register in a different order.
	ComponentHashes.Sort();

	uint32 CombinedHash = BuildVersion;
	for(const uint32 ComponentHash : ComponentHashes)
	{
		CombinedHash = HashCombine(CombinedHash, ComponentHash);
	}
	return CombinedHash;
}

#endif // WITH_EDITOR

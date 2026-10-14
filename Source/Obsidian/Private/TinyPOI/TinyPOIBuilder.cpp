// Copyright 2026 out of sCope team - intrxx

#include "TinyPOI/TinyPOIBuilder.h"

#if WITH_EDITOR

#include "Engine/Level.h"

#include "Obsidian/ObsidianLogCategories.h"
#include "TinyPOI/TinyPOIComponent.h"
#include "TinyPOI/TinyPOIData.h"
#include "TinyPOI/TinyPOITypes.h"


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
			UE_LOG(ObLogTinyPOI, Verbose, TEXT("[%s] is up to date, the bake is skipped."), *POIData->GetPathName());
			continue;
		}

		// Marks the level as dirty and makes the bake a part of the transaction that caused it, if there is one.
		POIData->Modify();

		FTinyPOIStorage& Storage = POIData->GetStorageMutable();
		BuildStorage(Level, Storage);
		POIData->SetCombinedHash(CombinedHash);

		UE_LOG(ObLogTinyPOI, Log, TEXT("Baked [%d] Points Of Interest with [%d] Tags into [%s]."), Storage.Points.Num(),
			Storage.TagRanges.Num(), *POIData->GetPathName());
	}

	bIsDirty = false;
}

void FTinyPOIBuilder::BuildStorage(const ULevel* InLevel, FTinyPOIStorage& OutStorage) const
{
	OutStorage.Reset();

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
		Point.Position = ComponentTransform.GetLocation();
		Point.Forward = ComponentTransform.GetUnitAxis(EAxis::X);
		Point.Radius = Component->GetRadius() * static_cast<float>(ComponentTransform.GetMaximumAxisScale());
		Point.Tag = Component->GetTag();
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
			if(InLeft.Position.X != InRight.Position.X)
			{
				return InLeft.Position.X < InRight.Position.X;
			}
			if(InLeft.Position.Y != InRight.Position.Y)
			{
				return InLeft.Position.Y < InRight.Position.Y;
			}
			return InLeft.Position.Z < InRight.Position.Z;
		});

	// Build the data that is derived from the Points: the range of every Tag and the bounds of the whole Storage.
	for(int32 PointIndex = 0; PointIndex < OutStorage.Points.Num(); ++PointIndex)
	{
		const FTinyPOIPoint& Point = OutStorage.Points[PointIndex];

		if(OutStorage.TagRanges.IsEmpty() || OutStorage.TagRanges.Last().Tag != Point.Tag)
		{
			FTinyPOITagRange& TagRange = OutStorage.TagRanges.AddDefaulted_GetRef();
			TagRange.Tag = Point.Tag;
			TagRange.PointsBegin = PointIndex;
		}
		OutStorage.TagRanges.Last().PointsEnd = PointIndex + 1;

		OutStorage.Bounds += FBox::BuildAABB(Point.Position, FVector(Point.Radius));
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

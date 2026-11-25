// Copyright 2026 out of sCope team - intrxx

#include "TinyPOI/TinyPOISubsystem.h"

#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/Level.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "HAL/IConsoleManager.h"

#include "Obsidian/ObsidianLogCategories.h"
#include "TinyPOI/TinyPOIComponent.h"
#include "TinyPOI/TinyPOIData.h"


namespace TinyPOIConsole
{
#if ENABLE_DRAW_DEBUG
	bool bDebugDraw = false;
	FAutoConsoleVariableRef CVarDebugDraw(TEXT("TinyPOI.DebugDraw"), bDebugDraw,
		TEXT("Draws the baked Points Of Interest of every loaded level, in the editor and in the game."));
#endif

#if WITH_EDITOR
	FAutoConsoleCommand RebuildCommand(TEXT("TinyPOI.Rebuild"),
		TEXT("Bakes the Points Of Interest of every level loaded in the editor again, even if nothing has changed."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			if(GEngine == nullptr)
			{
				return;
			}

			for(const FWorldContext& WorldContext : GEngine->GetWorldContexts())
			{
				if(WorldContext.WorldType == EWorldType::Editor)
				{
					if(UTinyPOISubsystem* POISubsystem = UWorld::GetSubsystem<UTinyPOISubsystem>(WorldContext.World()))
					{
						POISubsystem->RebuildData(true);
					}
				}
			}
		}));
#endif
}

FTinyPOIDataHandle UTinyPOISubsystem::RegisterPOIData(ATinyPOIData& InPOIData)
{
	if(IsValid(&InPOIData) == false)
	{
		return FTinyPOIDataHandle();
	}

	if(InPOIData.IsRegistered())
	{
		UE_LOG(ObLogTinyPOI, Error, TEXT("Trying to register [%s] which is already registered."), *InPOIData.GetName());
		return FTinyPOIDataHandle();
	}

	const int32 Index = RegisteredPOIDataFreeList.IsEmpty() ? RegisteredPOIData.AddDefaulted() : RegisteredPOIDataFreeList.Pop(EAllowShrinking::No);
	check(Index < MAX_uint16);

	FTinyPOIRegisteredData& RegisteredData = RegisteredPOIData[Index];
	RegisteredData.POIData = &InPOIData;

	const FTinyPOIDataHandle DataHandle(static_cast<uint16>(Index), static_cast<uint16>(RegisteredData.Generation));
	InPOIData.OnRegistered(DataHandle);

	UE_LOG(ObLogTinyPOI, Verbose, TEXT("Registered [%s] with [%d] Points Of Interest."), *InPOIData.GetPathName(), InPOIData.GetStorage().Points.Num());
	return DataHandle;
}

void UTinyPOISubsystem::UnregisterPOIData(ATinyPOIData& InPOIData)
{
	const int32 Index = RegisteredPOIData.IndexOfByPredicate([&InPOIData](const FTinyPOIRegisteredData& InRegisteredData)
		{
			return InRegisteredData.POIData == &InPOIData;
		});

	if(Index == INDEX_NONE)
	{
		UE_LOG(ObLogTinyPOI, Error, TEXT("Trying to unregister [%s] which is not registered."), *InPOIData.GetName());
		return;
	}

	// Bump the Generation, so the handles that were given out for this Data stop resolving once the index is reused.
	FTinyPOIRegisteredData& RegisteredData = RegisteredPOIData[Index];
	check(RegisteredData.Generation + 1 < MAX_uint16);
	RegisteredData.Reset(RegisteredData.Generation + 1);
	RegisteredPOIDataFreeList.Add(Index);

	InPOIData.OnUnregistered();
}

const FTinyPOIStorage* UTinyPOISubsystem::GetStorage(const FTinyPOIDataHandle InDataHandle) const
{
	if(RegisteredPOIData.IsValidIndex(InDataHandle.Index))
	{
		const FTinyPOIRegisteredData& RegisteredData = RegisteredPOIData[InDataHandle.Index];
		if(RegisteredData.POIData && RegisteredData.Generation == InDataHandle.Generation)
		{
			return &RegisteredData.POIData->GetStorage();
		}
	}
	return nullptr;
}

const FTinyPOIPoint* UTinyPOISubsystem::GetPoint(const FTinyPOIHandle InHandle) const
{
	const FTinyPOIStorage* Storage = GetStorage(InHandle.DataHandle);
	if(Storage && Storage->Points.IsValidIndex(InHandle.Index))
	{
		return &Storage->Points[InHandle.Index];
	}
	return nullptr;
}

int32 UTinyPOISubsystem::GetNumPoints() const
{
	int32 NumPoints = 0;
	for(const FTinyPOIRegisteredData& RegisteredData : RegisteredPOIData)
	{
		if(RegisteredData.POIData)
		{
			NumPoints += RegisteredData.POIData->GetStorage().Points.Num();
		}
	}
	return NumPoints;
}

bool UTinyPOISubsystem::FindNearestPoint(const FVector& InLocation, const float InMaxDistance, const FGameplayTag& InTagFilter,
	FTinyPOIHandle& OutHandle) const
{
	OutHandle.Reset();

	float NearestDistanceSqr = TNumericLimits<float>::Max();
	for(const FTinyPOIRegisteredData& RegisteredData : RegisteredPOIData)
	{
		if(RegisteredData.POIData == nullptr)
		{
			continue;
		}

		FTinyPOIHandle Handle;
		float DistanceSqr = 0.0f;
		if(RegisteredData.POIData->GetStorage().FindNearestPoint(InLocation, InMaxDistance, InTagFilter, Handle, DistanceSqr))
		{
			if(DistanceSqr < NearestDistanceSqr)
			{
				NearestDistanceSqr = DistanceSqr;
				OutHandle = Handle;
			}
		}
	}

	return OutHandle.IsValid();
}

bool UTinyPOISubsystem::FindPointsInRange(const FVector& InLocation, const float InRange, const FGameplayTag& InTagFilter,
	TArray<FTinyPOIHandle>& OutHandles) const
{
	OutHandles.Reset();

	for(const FTinyPOIRegisteredData& RegisteredData : RegisteredPOIData)
	{
		if(RegisteredData.POIData)
		{
			RegisteredData.POIData->GetStorage().FindPointsInRange(InLocation, InRange, InTagFilter, OutHandles);
		}
	}

	return OutHandles.IsEmpty() == false;
}

#if WITH_EDITOR
void UTinyPOISubsystem::RebuildData(const bool bInForceRebuild)
{
	const UWorld* World = GetWorld();
	if(World == nullptr || World->IsGameWorld())
	{
		return;
	}

	RegisterPOIDataInstances();
	SpawnMissingPOIData();

	TArray<ATinyPOIData*> AllPOIData;
	AllPOIData.Reserve(RegisteredPOIData.Num());
	for(const FTinyPOIRegisteredData& RegisteredData : RegisteredPOIData)
	{
		if(RegisteredData.POIData)
		{
			AllPOIData.Add(RegisteredData.POIData);
		}
	}

	Builder.BuildAll(AllPOIData, bInForceRebuild);
}
#endif // WITH_EDITOR

void UTinyPOISubsystem::Tick(float InDeltaTime)
{
#if WITH_EDITOR
	// Only markup that lives in the editor world registers in the Builder, so this never bakes during the game.
	if(Builder.NeedsRebuild())
	{
		RebuildData();
	}
#endif

#if ENABLE_DRAW_DEBUG
	if(TinyPOIConsole::bDebugDraw)
	{
		DebugDraw();
	}
#endif
}

TStatId UTinyPOISubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTinyPOISubsystem, STATGROUP_Tickables);
}

void UTinyPOISubsystem::RegisterPOIDataInstances()
{
	for(TActorIterator<ATinyPOIData> It(GetWorld()); It; ++It)
	{
		ATinyPOIData* POIData = *It;
		if(IsValid(POIData) && POIData->IsRegistered() == false)
		{
			RegisterPOIData(*POIData);
		}
	}
}

#if WITH_EDITOR
void UTinyPOISubsystem::SpawnMissingPOIData()
{
	// Every level that has markup...
	TSet<ULevel*> LevelsWithoutData;
	for(const TWeakObjectPtr<UTinyPOIComponent>& WeakComponent : Builder.GetRegisteredPOIComponents())
	{
		if(const UTinyPOIComponent* Component = WeakComponent.Get())
		{
			if(ULevel* Level = Component->GetComponentLevel())
			{
				LevelsWithoutData.Add(Level);
			}
		}
	}

	// ...but no Data yet. A level keeps its Data after the last markup is removed from it, it just gets baked empty.
	for(const FTinyPOIRegisteredData& RegisteredData : RegisteredPOIData)
	{
		if(RegisteredData.POIData)
		{
			LevelsWithoutData.Remove(RegisteredData.POIData->GetLevel());
		}
	}

	for(ULevel* Level : LevelsWithoutData)
	{
		// The Data lives in the level it is baked from, so it is saved, streamed and unloaded together with it. It
		// registers itself from PostActorCreated().
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.OverrideLevel = Level;
		GetWorld()->SpawnActor<ATinyPOIData>(SpawnParameters);
	}
}
#endif // WITH_EDITOR

#if ENABLE_DRAW_DEBUG
void UTinyPOISubsystem::DebugDraw() const
{
	const UWorld* World = GetWorld();

	for(const FTinyPOIRegisteredData& RegisteredData : RegisteredPOIData)
	{
		if(RegisteredData.POIData == nullptr)
		{
			continue;
		}

		const FTinyPOIStorage& Storage = RegisteredData.POIData->GetStorage();
		if(Storage.Bounds.IsValid)
		{
			DrawDebugBox(World, Storage.Bounds.GetCenter(), Storage.Bounds.GetExtent(), FColor::White);
		}

		for(const FTinyPOITagRange& TagRange : Storage.TagRanges)
		{
			const FColor Color = FColor::MakeRandomSeededColor(GetTypeHash(TagRange.Tag.ToString()));
			for(int32 PointIndex = TagRange.PointsBegin; PointIndex < TagRange.PointsEnd; ++PointIndex)
			{
				const FTinyPOIPoint& Point = Storage.Points[PointIndex];
				DrawDebugSphere(World, Point.Position, Point.Radius, 16, Color);
				DrawDebugDirectionalArrow(World, Point.Position, Point.Position + Point.Forward * Point.Radius, 20.0f, Color);
				DrawDebugString(World, Point.Position, TagRange.Tag.ToString(), nullptr, Color, 0.0f);
			}
		}
	}
}
#endif // ENABLE_DRAW_DEBUG

void UTinyPOISubsystem::PostInitialize()
{
	Super::PostInitialize();

	RegisterPOIDataInstances();
}

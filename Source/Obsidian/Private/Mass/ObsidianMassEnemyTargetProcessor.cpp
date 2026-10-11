// Copyright 2026 out of sCope team - intrxx

#include "Mass/ObsidianMassEnemyTargetProcessor.h"

#include "Kismet/GameplayStatics.h"
#include "MassActorSubsystem.h"
#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "MassSignalSubsystem.h"
#include "MassStateTreeTypes.h"

#include "Mass/ObsidianMassEnemyFragments.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"


UObsidianMassEnemyTargetProcessor::UObsidianMassEnemyTargetProcessor()
	: EntityQuery(*this)
{
	// Players are Actors, they can only be read on the game thread.
	bRequiresGameThreadExecution = true;
	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);

	// The State Trees need to see the Target of this frame.
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::SyncWorldToMass);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Behavior);
}

void UObsidianMassEnemyTargetProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& InEntityManager)
{
	EntityQuery.AddTagRequirement<FObsidianMassEnemyTag>(EMassFragmentPresence::All);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FObsidianMassEnemyStateFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FObsidianMassCombatTargetFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FObsidianMassEnemyParameters>(EMassFragmentPresence::All);
	EntityQuery.AddSubsystemRequirement<UMassSignalSubsystem>(EMassFragmentAccess::ReadWrite);

	// The entities are signaled after all of them are processed, which is done outside of the query.
	ProcessorRequirements.AddSubsystemRequirement<UMassSignalSubsystem>(EMassFragmentAccess::ReadWrite);
}

void UObsidianMassEnemyTargetProcessor::Execute(FMassEntityManager& InEntityManager, FMassExecutionContext& InContext)
{
	TArray<AActor*> Players;
	bool bPlayersGathered = false;
	TArray<FMassEntityHandle> EntitiesToSignal;

	EntityQuery.ForEachEntityChunk(InContext, [&Players, &bPlayersGathered, &EntitiesToSignal](FMassExecutionContext& InChunkContext)
		{
			const float DeltaTime = InChunkContext.GetDeltaTimeSeconds();
			const FObsidianMassEnemyParameters& Parameters = InChunkContext.GetConstSharedFragment<FObsidianMassEnemyParameters>();
			const TConstArrayView<FTransformFragment> Transforms = InChunkContext.GetFragmentView<FTransformFragment>();
			const TConstArrayView<FMassActorFragment> Actors = InChunkContext.GetFragmentView<FMassActorFragment>();
			const TConstArrayView<FObsidianMassEnemyStateFragment> States = InChunkContext.GetFragmentView<FObsidianMassEnemyStateFragment>();
			const TArrayView<FObsidianMassCombatTargetFragment> Targets = InChunkContext.GetMutableFragmentView<FObsidianMassCombatTargetFragment>();

			for(FMassExecutionContext::FEntityIterator EntityIt = InChunkContext.CreateEntityIterator(); EntityIt; ++EntityIt)
			{
				FObsidianMassCombatTargetFragment& Target = Targets[EntityIt];
				Target.TimeToNextUpdate -= DeltaTime;
				if(Target.TimeToNextUpdate > 0.0f || States[EntityIt].bDying)
				{
					continue;
				}
				Target.TimeToNextUpdate = FMath::FRandRange(FMath::Max(0.0f, Parameters.TargetUpdateInterval - Parameters.TargetUpdateRandomDeviation),
					Parameters.TargetUpdateInterval + Parameters.TargetUpdateRandomDeviation);

				if(bPlayersGathered == false)
				{
					bPlayersGathered = true;
					UGameplayStatics::GetAllActorsWithTag(InChunkContext.GetWorld(), ObsidianActorTags::Player, Players);
				}

				// Hydrated Enemies measure from their Actor, so the distances are the same ones the Behavior Trees worked with.
				const AActor* EnemyActor = Actors[EntityIt].Get();
				const FVector EnemyLocation = EnemyActor ? EnemyActor->GetActorLocation() : Transforms[EntityIt].GetTransform().GetLocation();

				AActor* NearestPlayer = nullptr;
				double NearestDistanceSquared = TNumericLimits<double>::Max();
				for(AActor* Player : Players)
				{
					const double DistanceSquared = EnemyActor ? FVector::DistSquared(EnemyLocation, Player->GetActorLocation())
						: FVector::DistSquared2D(EnemyLocation, Player->GetActorLocation());
					if(DistanceSquared < NearestDistanceSquared)
					{
						NearestDistanceSquared = DistanceSquared;
						NearestPlayer = Player;
					}
				}

				const bool bTargetChanged = Target.TargetActor.Get() != NearestPlayer;
				Target.TargetActor = NearestPlayer;
				Target.TargetLocation = NearestPlayer ? NearestPlayer->GetActorLocation() : FVector::ZeroVector;
				Target.DistanceToTarget = NearestPlayer ? static_cast<float>(FMath::Sqrt(NearestDistanceSquared)) : UE_FLOAT_HUGE_DISTANCE;

				if(bTargetChanged || Target.DistanceToTarget <= Parameters.StateTreeWakeUpDistance)
				{
					EntitiesToSignal.Add(InChunkContext.GetEntity(EntityIt));
				}
			}
		});

	if(EntitiesToSignal.IsEmpty() == false)
	{
		UMassSignalSubsystem& SignalSubsystem = InContext.GetMutableSubsystemChecked<UMassSignalSubsystem>();
		SignalSubsystem.SignalEntities(UE::Mass::Signals::NewStateTreeTaskRequired, EntitiesToSignal);
	}
}

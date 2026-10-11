// Copyright 2026 out of sCope team - intrxx

#include "Mass/ObsidianMassEnemySubsystems.h"

#include "Engine/World.h"
#include "MassEntityManager.h"

#include "Characters/Enemies/ObsidianRegularEnemy.h"
#include "Mass/ObsidianMassEnemyFragments.h"


ESpawnRequestStatus UObsidianMassEnemyActorSpawnerSubsystem::SpawnActor(const FSpawnArgs& InArgs, FActorSpawnParameters& InOutSpawnParameters) const
{
	const FMassEntityHandle Entity = InArgs.Request.MassAgent;
	const FMassEntityManager& EntityManager = InArgs.EntityManager;

	InOutSpawnParameters.CustomPreSpawnInitialization = [Entity, &EntityManager](AActor* InActor)
		{
			AObsidianRegularEnemy* Enemy = Cast<AObsidianRegularEnemy>(InActor);
			if(Enemy == nullptr || EntityManager.IsEntityValid(Entity) == false)
			{
				return;
			}

			const FObsidianMassEnemyIdentityFragment* Identity = EntityManager.GetFragmentDataPtr<FObsidianMassEnemyIdentityFragment>(Entity);
			Enemy->InitializeFromMassEntity(Entity, Identity ? *Identity : FObsidianMassEnemyIdentityFragment());
		};

	return Super::SpawnActor(InArgs, InOutSpawnParameters);
}

void UObsidianMassEnemyRepresentationSubsystem::Initialize(FSubsystemCollectionBase& InCollection)
{
	InCollection.InitializeDependency(UObsidianMassEnemyActorSpawnerSubsystem::StaticClass());

	Super::Initialize(InCollection);

	ActorSpawnerSubsystem = UWorld::GetSubsystem<UObsidianMassEnemyActorSpawnerSubsystem>(GetWorld());
}

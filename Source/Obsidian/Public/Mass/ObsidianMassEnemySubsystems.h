// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "MassActorSpawnerSubsystem.h"
#include "MassRepresentationSubsystem.h"

#include "ObsidianMassEnemySubsystems.generated.h"

/**
 * Spawns the Actors the Enemy entities are hydrated into. Gives the Enemy its entity and identity before it begins
 * play, as this is when the Enemy initializes its Ability System with them.
 */
UCLASS()
class OBSIDIAN_API UObsidianMassEnemyActorSpawnerSubsystem : public UMassActorSpawnerSubsystem
{
	GENERATED_BODY()

protected:
	//~ Begin UMassActorSpawnerSubsystem Interface
	virtual ESpawnRequestStatus SpawnActor(const FSpawnArgs& InArgs, FActorSpawnParameters& InOutSpawnParameters) const override;
	//~ End UMassActorSpawnerSubsystem Interface
};

/**
 * Representation Subsystem of the Enemy entities, set on the visualization trait of their entity config. The only
 * difference from the base one is the Actor Spawner it uses.
 */
UCLASS()
class OBSIDIAN_API UObsidianMassEnemyRepresentationSubsystem : public UMassRepresentationSubsystem
{
	GENERATED_BODY()

protected:
	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& InCollection) override;
	//~ End USubsystem Interface
};

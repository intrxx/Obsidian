// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "MassEntityElementTypes.h"

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "ObsidianMassEnemyFragments.generated.h"

/**
 * Identifies the Mass entity as an Obsidian Enemy.
 */
USTRUCT()
struct FObsidianMassEnemyTag : public FMassTag
{
	GENERATED_BODY()
};

/**
 * What stays with the Enemy for its whole life, no matter if it has an Actor at the moment. Pushed into the Actor
 * (and its Ability System) every time the entity gets hydrated.
 */
USTRUCT()
struct OBSIDIAN_API FObsidianMassEnemyIdentityFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Obsidian")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, Category = "Obsidian")
	EObsidianEntityRarity Rarity = EObsidianEntityRarity::Normal;
};

/**
 * Target the Enemy fights, updated by UObsidianMassEnemyTargetProcessor and read by the State Tree of the Enemy.
 */
USTRUCT()
struct OBSIDIAN_API FObsidianMassCombatTargetFragment : public FMassFragment
{
	GENERATED_BODY()

	bool HasTarget() const { return TargetActor.IsValid(); }

	TWeakObjectPtr<AActor> TargetActor;

	FVector TargetLocation = FVector::ZeroVector;

	/** Huge when there is no Target, the same value the Find Nearest Player Service of the Behavior Trees used. */
	float DistanceToTarget = UE_FLOAT_HUGE_DISTANCE;

	float TimeToNextUpdate = 0.0f;
};

/**
 * State of the hydrated Actor that the State Tree of the Enemy needs to know about, written by the Actor.
 */
USTRUCT()
struct OBSIDIAN_API FObsidianMassEnemyStateFragment : public FMassFragment
{
	GENERATED_BODY()

	bool bHitReacting = false;

	bool bDying = false;
};

/**
 * Parameters shared by every Enemy created from the same entity config.
 */
USTRUCT()
struct OBSIDIAN_API FObsidianMassEnemyParameters : public FMassConstSharedFragment
{
	GENERATED_BODY()

	/** Time between the searches for the nearest Player. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ForceUnits = "s"), Category = "Obsidian")
	float TargetUpdateInterval = 0.5f;

	/** Adds random range to the Target Update Interval. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ForceUnits = "s"), Category = "Obsidian")
	float TargetUpdateRandomDeviation = 0.1f;

	/** State Tree of the Enemy is woken up on every Target update as long as the Target is closer than this. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ForceUnits = "cm"), Category = "Obsidian")
	float StateTreeWakeUpDistance = 3000.0f;
};

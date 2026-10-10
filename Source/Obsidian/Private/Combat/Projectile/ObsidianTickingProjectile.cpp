// Copyright 2026 out of sCope team - intrxx

#include "Combat/Projectile/ObsidianTickingProjectile.h"


AObsidianTickingProjectile::AObsidianTickingProjectile(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AObsidianTickingProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (ProjectileCleanupMethod == EObsidianProjectileCleanupMethod::LifeSpan)
	{
		SetLifeSpan(ProjectileLifeSpan);
	}
	else if (ProjectileCleanupMethod == EObsidianProjectileCleanupMethod::DistanceTraveled)
	{
		ProjectileSpawnLocation = GetActorLocation();
	}
}

void AObsidianTickingProjectile::Tick(float InDeltaSeconds)
{
	Super::Tick(InDeltaSeconds);

	if (ProjectileCleanupMethod == EObsidianProjectileCleanupMethod::DistanceTraveled)
	{
		if (DistanceToTravel < FVector::Dist(ProjectileSpawnLocation, GetActorLocation()))
		{
			Destroy();
		}
	}
}

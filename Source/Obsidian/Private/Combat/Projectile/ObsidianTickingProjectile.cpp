// Copyright 2026 out of sCope team - intrxx

#include "Combat/Projectile/ObsidianTickingProjectile.h"

AObsidianTickingProjectile::AObsidianTickingProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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

void AObsidianTickingProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ProjectileCleanupMethod == EObsidianProjectileCleanupMethod::DistanceTraveled)
	{
		if (DistanceToTravel < FVector::Dist(ProjectileSpawnLocation, GetActorLocation()))
		{
			Destroy();
		}
	}
}

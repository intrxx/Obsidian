// Copyright 2026 out of sCope team - intrxx

#include "Combat/Projectile/ObsidianProjectile.h"

#include "Combat/Projectile/OProjectileMovementComponent.h"


AObsidianProjectile::AObsidianProjectile(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AObsidianProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (ProjectileCleanupMethod != EObsidianProjectileCleanupMethod::Custom)
	{
		float LifeSpan = ProjectileLifeSpan;
		if (ProjectileCleanupMethod == EObsidianProjectileCleanupMethod::DistanceTraveled)
		{
			LifeSpan = DistanceToTravel / ProjectileMovementComponent->InitialSpeed;
		}
		
		SetLifeSpan(LifeSpan);
	}
}

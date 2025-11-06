// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>


#include "Combat/Projectile/ObsidianProjectileBase.h"
#include "ObsidianTickingProjectile.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class OBSIDIAN_API AObsidianTickingProjectile : public AObsidianProjectileBase
{
	GENERATED_BODY()

public:
	AObsidianTickingProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	FVector ProjectileSpawnLocation = FVector::ZeroVector;
};

// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

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
	AObsidianTickingProjectile(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	//~ Begin AActor Interface
	virtual void Tick(float InDeltaSeconds) override;
	//~ End AActor Interface

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	FVector ProjectileSpawnLocation = FVector::ZeroVector;
};

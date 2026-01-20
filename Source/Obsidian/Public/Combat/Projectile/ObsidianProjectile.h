// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>


#include "Combat/Projectile/ObsidianProjectileBase.h"
#include "ObsidianProjectile.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class OBSIDIAN_API AObsidianProjectile : public AObsidianProjectileBase
{
	GENERATED_BODY()

public:
	AObsidianProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
};

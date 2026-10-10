// Copyright 2026 out of sCope - intrxx

#pragma once

#include "CoreMinimal.h"

#include "CharacterComponents/Movement/ObsidianCharacterMovementComponent.h"

#include "ObsidianEnemyMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianEnemyMovementComponent : public UObsidianCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UObsidianEnemyMovementComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
		
	virtual float GetMaxSpeed() const override;
};

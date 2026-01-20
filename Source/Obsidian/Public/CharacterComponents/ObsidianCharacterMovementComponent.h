// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "GameFramework/CharacterMovementComponent.h"
#include "ObsidianCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UObsidianCharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual float GetMaxSpeed() const override;
	
};



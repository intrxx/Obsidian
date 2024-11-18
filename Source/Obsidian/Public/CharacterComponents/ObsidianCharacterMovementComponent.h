// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
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



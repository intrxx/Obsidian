// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "GameplayModMagnitudeCalculation.h"
#include "ObsidianMMC_ChaosResistance.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMMC_ChaosResistance : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UObsidianMMC_ChaosResistance();
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	
};

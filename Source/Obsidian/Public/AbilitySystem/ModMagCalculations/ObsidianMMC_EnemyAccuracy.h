// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "GameplayModMagnitudeCalculation.h"
#include "ObsidianMMC_EnemyAccuracy.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMMC_EnemyAccuracy : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override; 
};

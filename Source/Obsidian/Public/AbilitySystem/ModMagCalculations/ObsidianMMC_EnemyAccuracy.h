// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>


#include <GameplayModMagnitudeCalculation.h>
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

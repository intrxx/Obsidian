// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>


#include <GameplayModMagnitudeCalculation.h>
#include "ObsidianMMC_ChaosResistance.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMMC_ChaosResistance : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UObsidianMMC_ChaosResistance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	
};

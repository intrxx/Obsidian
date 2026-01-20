// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>


#include <GameplayModMagnitudeCalculation.h>
#include "ObsidianMMC_MaxEnergyShield.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMMC_MaxEnergyShield : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UObsidianMMC_MaxEnergyShield(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "ObsidianMMC_Evasion.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMMC_Evasion : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UObsidianMMC_Evasion();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

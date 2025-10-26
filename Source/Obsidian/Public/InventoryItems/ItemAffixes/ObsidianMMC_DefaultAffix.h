// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "ObsidianMMC_DefaultAffix.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMMC_DefaultAffix : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UObsidianMMC_DefaultAffix(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

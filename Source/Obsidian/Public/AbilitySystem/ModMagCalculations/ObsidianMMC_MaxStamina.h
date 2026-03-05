// Copyright 2026 out of sCope - intrxx

#pragma once

#include <CoreMinimal.h>


#include <GameplayModMagnitudeCalculation.h>
#include "ObsidianMMC_MaxStamina.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMMC_MaxStamina : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()


public:
	UObsidianMMC_MaxStamina(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Obsidian")
	TObjectPtr<UCurveTable> MaxStaminaBonusCurveTable;
};

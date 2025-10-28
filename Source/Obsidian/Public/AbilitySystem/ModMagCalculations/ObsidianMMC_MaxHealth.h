// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>


#include <GameplayModMagnitudeCalculation.h>
#include "ObsidianMMC_MaxHealth.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMMC_MaxHealth : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UObsidianMMC_MaxHealth(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Obsidian")
	TObjectPtr<UCurveTable> MaxHealthAwardCurveTable;
};

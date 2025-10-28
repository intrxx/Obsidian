// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>


#include <GameplayModMagnitudeCalculation.h>
#include "ObsidianMMC_MaxSpecialResource.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMMC_MaxSpecialResource : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Obsidian")
	TObjectPtr<UCurveTable> MaxSpecialResourceAwardCurveTable;
};

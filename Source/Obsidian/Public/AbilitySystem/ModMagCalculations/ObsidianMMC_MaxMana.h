// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>


#include <GameplayModMagnitudeCalculation.h>
#include "ObsidianMMC_MaxMana.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMMC_MaxMana : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UObsidianMMC_MaxMana(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Obsidian")
	TObjectPtr<UCurveTable> MaxManaAwardCurveTable;
};

// Copyright 2026 out of sCope - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"

#include "ObsidianMMC_MaxStamina.generated.h"

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API UObsidianMMC_MaxStamina : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UObsidianMMC_MaxStamina(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	//~ Begin UGameplayModMagnitudeCalculation Interface
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& InSpec) const override;
	//~ End UGameplayModMagnitudeCalculation Interface

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Obsidian")
	TObjectPtr<UCurveTable> MaxStaminaBonusCurveTable;
};

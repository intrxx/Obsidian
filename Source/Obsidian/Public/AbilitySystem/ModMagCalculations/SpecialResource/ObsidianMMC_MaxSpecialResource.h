// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"

#include "ObsidianMMC_MaxSpecialResource.generated.h"

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API UObsidianMMC_MaxSpecialResource : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	//~ Begin UGameplayModMagnitudeCalculation Interface
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& InSpec) const override;
	//~ End UGameplayModMagnitudeCalculation Interface

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Obsidian")
	TObjectPtr<UCurveTable> MaxSpecialResourceAwardCurveTable;
};

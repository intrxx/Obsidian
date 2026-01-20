// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>


#include "AbilitySystem/ModMagCalculations/SpecialResource/ObsidianMMC_MaxSpecialResource.h"
#include "ObsidianMMC_MaxSpecialResource_Witch.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMMC_MaxSpecialResource_Witch : public UObsidianMMC_MaxSpecialResource
{
	GENERATED_BODY()

public:
	UObsidianMMC_MaxSpecialResource_Witch(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

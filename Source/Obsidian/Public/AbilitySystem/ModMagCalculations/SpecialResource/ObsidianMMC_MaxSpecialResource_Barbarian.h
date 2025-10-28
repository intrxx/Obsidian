// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>


#include "AbilitySystem/ModMagCalculations/SpecialResource/ObsidianMMC_MaxSpecialResource.h"
#include "ObsidianMMC_MaxSpecialResource_Barbarian.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMMC_MaxSpecialResource_Barbarian : public UObsidianMMC_MaxSpecialResource
{
	GENERATED_BODY()

public:
	UObsidianMMC_MaxSpecialResource_Barbarian(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

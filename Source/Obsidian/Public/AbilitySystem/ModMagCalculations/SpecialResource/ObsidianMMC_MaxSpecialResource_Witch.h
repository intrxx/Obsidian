// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


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
	UObsidianMMC_MaxSpecialResource_Witch();
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>


#include "AbilitySystem/Abilities/ObsidianDamageGameplayAbility.h"
#include "ObsidianGameplayAbility_Melee.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianGameplayAbility_Melee : public UObsidianDamageGameplayAbility
{
	GENERATED_BODY()

protected:
	FGameplayEffectSpecHandle MakeMeleeDamageSpec(const UObject* SourceObject);
};

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ObsidianGameplayAbility.h"
#include "ObsidianDamageGameplayAbility.generated.h"

USTRUCT()
struct FObsidianAbilityDamageRange
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FScalableFloat MinimalDamage;

	UPROPERTY(EditDefaultsOnly)
	FScalableFloat MaximalDamage;

	float RollForDamageNumberAtLevel(const float Level) const;
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianDamageGameplayAbility : public UObsidianGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Damage")
	FObsidianAbilityDamageRange AbilityDamageRange;
};

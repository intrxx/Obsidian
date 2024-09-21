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
 * Base class in Obsidian for abilities that wish to cause damage
 */
UCLASS()
class OBSIDIAN_API UObsidianDamageGameplayAbility : public UObsidianGameplayAbility
{
	GENERATED_BODY()

public:
	UObsidianDamageGameplayAbility();

protected:
	/** Given the Array of Actors, damages all of them with the damage provided in the ability. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|DamageAbility")
	void DamageAllCharacters(UPARAM(ref) TArray<AActor*>& ActorsToDamage);

	/** Damages the actor with the damage provided in the ability. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|DamageAbility")
	void DamageCharacter(AActor* ActorToDamage);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Damage")
	TMap<FGameplayTag, FObsidianAbilityDamageRange> DamageTypeMap;

	/** Damage effect that will be applied to target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|AbilitySetup")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};

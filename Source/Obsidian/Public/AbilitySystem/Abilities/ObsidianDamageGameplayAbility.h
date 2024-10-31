// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ObsidianGameplayAbility.h"
#include "ObsidianDamageGameplayAbility.generated.h"

class UNiagaraSystem;

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
	void DamageAllCharacters(const TArray<AActor*>& ActorsToDamage);

	/** Damages the actor with the damage provided in the ability. Uses DamageTypeMap. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|DamageAbility")
	void DamageCharacter(AActor* ActorToDamage);

	/** Spawns blood effect at given BloodOriginLocation if one exists on the specified Actor. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|DamageAbility")
	void SpawnBloodEffect(AActor* Actor, const FVector& BloodOriginLocation, UNiagaraSystem* OptionalBloodEffect = nullptr);

	/** Spawns blood effects if they exist on specified actors. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|DamageAbility")
	void SpawnBloodEffects(const TArray<AActor*>& Actors, UNiagaraSystem* OptionalBloodEffect = nullptr);

	/** Combines damaging all character and spawning blood effects at their locations if one exist on provided actors. May be slightly more performant. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|DamageAbility")
	void DamageAllCharactersAndSpawnBloodEffect(const TArray<AActor*>& Actors, UNiagaraSystem* OptionalBloodEffect = nullptr);
	
protected:
	// UPROPERTY(EditDefaultsOnly, meta=(Categories = "SetByCaller.DamageType"), Category = "Obsidian|Damage")
	//TODO Categorisation of gameplay tags inside a TMap does not work for now.
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Damage")
	TMap<FGameplayTag, FObsidianAbilityDamageRange> BaseDamageTypeMap;

	/** Damage effect that will be applied to target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|AbilitySetup")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};

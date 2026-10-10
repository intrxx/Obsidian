// Copyright 2026 out of sCope team - intrxx

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

	float RollForDamageNumberAtLevel(const float InLevel) const;
};

/**
 * Base class in Obsidian for abilities that wish to cause damage
 */
UCLASS()
class OBSIDIAN_API UObsidianDamageGameplayAbility : public UObsidianGameplayAbility
{
	GENERATED_BODY()

public:
	UObsidianDamageGameplayAbility(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

protected:
	/** Given the Array of Actors, damages all of them with the damage provided in the ability. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|DamageAbility")
	void DamageAllCharacters(const TArray<AActor*>& InActorsToDamage);

	/** Damages the actor with the damage provided in the ability. Uses DamageTypeMap. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|DamageAbility")
	void DamageCharacter(AActor* InActorToDamage);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|DamageAbility")
	static FVector PredictActorLocation(AActor* InActor, const float InTime = 1.0f, const FVector& InFallBackVector = FVector::ZeroVector);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|DamageAbility")
	static FVector ShortenVector(const FVector& InStartVector, const FVector& InEndVector, const float InAmountToShorten);
	
protected:
	// UPROPERTY(EditDefaultsOnly, meta=(Categories = "SetByCaller.DamageType"), Category = "Obsidian|Damage")
	//TODO(intrxx) Categorisation of gameplay tags inside a TMap does not work for now.
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Damage")
	TMap<FGameplayTag, FObsidianAbilityDamageRange> BaseDamageTypeMap;

	/** Damage effect that will be applied to target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|AbilitySetup")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};

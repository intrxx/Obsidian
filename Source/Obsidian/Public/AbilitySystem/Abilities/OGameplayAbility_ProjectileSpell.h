// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ObsidianGameplayAbility.h"
#include "ObsidianTypes/ObsidianAbilitySpawnLocation.h"
#include "OGameplayAbility_ProjectileSpell.generated.h"

class AObsidianProjectile;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UOGameplayAbility_ProjectileSpell : public UObsidianGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|ProjectileSpell")
	void SpawnProjectile(const FVector& TargetLocation);
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	FVector GetSpawnLocation() const; 

protected:
	/** Enum to decide at what location the projectile will be spawned */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|AbilitySetup")
	EObsidianAbilitySpawnLocation AbilitySpawnLocation;
	
	/** Projectile class to spawn by this ability */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|AbilitySetup")
	TSubclassOf<AObsidianProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|AbilitySetup")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};


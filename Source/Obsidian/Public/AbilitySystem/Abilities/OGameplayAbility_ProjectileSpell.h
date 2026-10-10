// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "ObsidianDamageGameplayAbility.h"

#include "OGameplayAbility_ProjectileSpell.generated.h"

class AObsidianProjectileBase;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UOGameplayAbility_ProjectileSpell : public UObsidianDamageGameplayAbility
{
	GENERATED_BODY()
public:
	UOGameplayAbility_ProjectileSpell(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|ProjectileSpell")
	void SpawnProjectile(const FVector& InSpawnLocation, const FVector& InTargetLocation, const bool bInWithDebug);
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, meta=(Categories="SetByCaller.DamageType"), Category = "Obsidian|Damage")
	TMap<FGameplayTag, FObsidianAbilityDamageRange> ProjectileDamageTypeMap;
	
	/** Projectile class to spawn by this ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|AbilitySetup")
	TSubclassOf<AObsidianProjectileBase> ProjectileClass;
};


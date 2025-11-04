// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>


#include "ObsidianDamageGameplayAbility.h"
#include "OGameplayAbility_ProjectileSpell.generated.h"

class AObsidianProjectile;

DECLARE_LOG_CATEGORY_EXTERN(LogProjectileAbility, Log, All);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UOGameplayAbility_ProjectileSpell : public UObsidianDamageGameplayAbility
{
	GENERATED_BODY()
public:
	UOGameplayAbility_ProjectileSpell(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|ProjectileSpell")
	void SpawnProjectile(const FVector& SpawnLocation, const FVector& TargetLocation, const bool bWithDebug);
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, meta=(Categories="SetByCaller.DamageType"), Category = "Obsidian|Damage")
	TMap<FGameplayTag, FObsidianAbilityDamageRange> ProjectileDamageTypeMap;
	
	/** Projectile class to spawn by this ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|AbilitySetup")
	TSubclassOf<AObsidianProjectile> ProjectileClass;
};


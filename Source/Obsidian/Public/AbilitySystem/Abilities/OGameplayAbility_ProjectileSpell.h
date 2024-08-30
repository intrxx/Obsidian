// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianDamageGameplayAbility.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "OGameplayAbility_ProjectileSpell.generated.h"

class AObsidianProjectile;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UOGameplayAbility_ProjectileSpell : public UObsidianDamageGameplayAbility
{
	GENERATED_BODY()
public:
	UOGameplayAbility_ProjectileSpell();
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|ProjectileSpell")
	void SpawnProjectile(const FVector& SpawnLocation, const FVector& TargetLocation);
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	/** Enum to decide at what location the projectile will be spawned */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|AbilitySetup")
	EObsidianAbilitySpawnLocation AbilitySpawnLocation = EObsidianAbilitySpawnLocation::ASL_DefaultLocation;
	
	/** Projectile class to spawn by this ability */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|AbilitySetup")
	TSubclassOf<AObsidianProjectile> ProjectileClass;
};


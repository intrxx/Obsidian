// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianDamageGameplayAbility.h"
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
	/** Projectile class to spawn by this ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|AbilitySetup")
	TSubclassOf<AObsidianProjectile> ProjectileClass;

	/** This class will be ignored by the projectile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|AbilitySetup")
	UClass* ClassToIgnore = nullptr;
};


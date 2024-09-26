// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "ObsidianProjectile.generated.h"

class UNiagaraSystem;
class UOProjectileMovementComponent;
class USphereComponent;

UCLASS()
class OBSIDIAN_API AObsidianProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AObsidianProjectile();

	virtual void Destroyed() override;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Projectile")
	UOProjectileMovementComponent* GetObsidianProjectileMovementComponent()
	{
		return ObsidianProjectileMovementComponent;
	}

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Projectile")
	FGameplayEffectSpecHandle GetProjectileDamageSpecHandle() const
	{
		return DamageEffectSpecHandle;
	}
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Projectile")
	void SetProjectileDamageSpecHandle(const FGameplayEffectSpecHandle DamageSpec)
	{
		DamageEffectSpecHandle = DamageSpec;
	}

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian|Projectile")
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Obsidian|Projectile")
	TObjectPtr<UOProjectileMovementComponent> ObsidianProjectileMovementComponent;
	
	/**
	 * Boolean to indicate that the projectile hit on the server, used to play cosmetic effects if the destruction happened on the server before the overlap on client.
	 * In other words, if the projectile is destroyed and the bServerHit is false, the cosmetic effects will be played in the Destroyed() function.
	 */
	bool bServerHit = false;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Projectile")
	float ProjectileLifeSpan = 15.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Projectile")
	bool bDestroyOnHit = true;
	
	/**
	 * Cosmetics
	 */

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Projectile|Cosmetics")
	TObjectPtr<UNiagaraSystem> ProjImpactEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Projectile|Cosmetics")
	TObjectPtr<USoundBase> ProjImpactSound;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Projectile|Cosmetics")
	TObjectPtr<USoundBase> ProjFlyingSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> ProjFlyingSoundComp;
	
	/**
	 * 
	 */

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true, AllowPrivateAccess = true), Category = "Obsidian|Projectile")
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Projectile")
	UClass* ClassToIgnore = nullptr;

	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;
};

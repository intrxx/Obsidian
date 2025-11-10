// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "GameplayEffectTypes.h"

#include "GameFramework/Actor.h"
#include "ObsidianProjectileBase.generated.h"

class AObsidianHero;

UENUM()
enum class EObsidianProjectileCleanupMethod : uint8
{
	/** The projectile will be cleaned up after set LifeSpan. */
	LifeSpan,

	/**
	 * The Projectile will be cleaned up after the set Distance Traveled,
	 * remember that this will only be accurate for ticking projectiles.
	 */
	DistanceTraveled,

	/** Custom way of cleaning the projectile, it needs to be provided in child class. */
	Custom
};

class UNiagaraSystem;
class UOProjectileMovementComponent;
class USphereComponent;

UCLASS(Abstract, NotBlueprintable)
class OBSIDIAN_API AObsidianProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AObsidianProjectileBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void Destroyed() override;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Projectile")
	UOProjectileMovementComponent* GetObsidianProjectileMovementComponent()
	{
		return ProjectileMovementComponent;
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
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian|Projectile")
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Obsidian|Projectile")
	TObjectPtr<UOProjectileMovementComponent> ProjectileMovementComponent;
	
	/**
	 * Boolean to indicate that the projectile hit on the server, used to play cosmetic effects if the destruction happened on the server before the overlap on client.
	 * In other words, if the projectile is destroyed and the bServerHit is false, the cosmetic effects will be played in the Destroyed() function.
	 */
	bool bServerHit = false;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Projectile")
	EObsidianProjectileCleanupMethod ProjectileCleanupMethod = EObsidianProjectileCleanupMethod::LifeSpan;

	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition="ProjectileCleanupMethod==EObsidianProjectileCleanupMethod::LifeSpan"), Category = "Obsidian|Projectile")
	float ProjectileLifeSpan = 15.f;
	
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition="ProjectileCleanupMethod==EObsidianProjectileCleanupMethod::DistanceTraveled"), Category = "Obsidian|Projectile")
	float DistanceToTravel = 300.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Projectile")
	bool bDestroyOnHit = true;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Projectile")
	bool bAllowMultiHit = false;
	
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

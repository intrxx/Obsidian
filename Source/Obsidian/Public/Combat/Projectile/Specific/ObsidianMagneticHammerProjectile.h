// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "Combat/Projectile/ObsidianTickingProjectile.h"

#include "ObsidianMagneticHammerProjectile.generated.h"

class UTimelineComponent;
class USplineComponent;
/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianMagneticHammerProjectile : public AObsidianTickingProjectile
{
	GENERATED_BODY()

public:
	AObsidianMagneticHammerProjectile(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	//~ Begin AActor Interface
	virtual void Tick(float InDeltaTime) override;
	//~ End AActor Interface

protected:
	void OnHammerRouteFinished();

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

private:
	void UpdateHammerRoute(const float InDeltaTime);

	UPROPERTY(VisibleDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Obsidian")
	TObjectPtr<UStaticMeshComponent> HammerMeshComponent;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Obsidian")
	TObjectPtr<UCurveFloat> HammerRouteCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float HammerRotationSpeed = 400.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float HammerRange = 700.0f;

	UPROPERTY()
	TWeakObjectPtr<AObsidianHero> HeroOwner = nullptr;

	FVector HammerMidwayLocation = FVector::ZeroVector;
	FVector HammerInitialScaleValue = FVector::ZeroVector;
	uint8 bReturning:1;
	float HammerMaxRotationSpeed = 0.0f;
	float HammerMaxSpeed = 0.0f;
	float HammerScaleValue = 0.0f;
};


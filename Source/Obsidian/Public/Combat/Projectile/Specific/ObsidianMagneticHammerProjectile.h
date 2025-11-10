// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Combat/Projectile/ObsidianTickingProjectile.h"
#include "ObsidianMagneticHammerProjectile.generated.h"

class UTimelineComponent;
class USplineComponent;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianMagneticHammerProjectile : public AObsidianTickingProjectile
{
	GENERATED_BODY()

public:
	AObsidianMagneticHammerProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
protected:
	void StartHammerRoute();

	UFUNCTION()
	void UpdateHammerRoute(float UpdateAlpha);
	void OnHammerRouteFinished();

private:
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<USplineComponent> HammerRouteSplineComponent;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UTimelineComponent> HammerRouteTimelineComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> HammerRouteCurve;

	FVector HammerMidwayLocation = FVector::ZeroVector;
	float HammerDistance = 500.0f;
	uint8 bReturning:1;
};


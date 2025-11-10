// Copyright 2024 out of sCope team - Michał Ogiński

#include "Combat/Projectile/Specific/ObsidianMagneticHammerProjectile.h"

#include <Components/SplineComponent.h>
#include <Components/TimelineComponent.h>

#include "Characters/Player/ObsidianPlayerState.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "Combat/Projectile/OProjectileMovementComponent.h"
#include "Core/ObsidianBlueprintFunctionLibrary.h"
#include "Obsidian/ObsidianMacros.h"

AObsidianMagneticHammerProjectile::AObsidianMagneticHammerProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HammerRouteTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("HammerRouteTimeline"));
	HammerRouteTimelineComponent->SetLooping(false);
	
	ProjectileMovementComponent->bAutoActivate = false;
	ProjectileCleanupMethod = EObsidianProjectileCleanupMethod::Custom;
	bDestroyOnHit = false;
}

void AObsidianMagneticHammerProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (ProjectileMovementComponent->IsActive())
	{
		UE_LOG(LogTemp, Display, TEXT("Movement Comp is Active"));
	}
	
	if (HasAuthority())
	{
		StartHammerRoute();
	}
}

void AObsidianMagneticHammerProjectile::StartHammerRoute()
{
	if (HammerRouteSplineComponent == nullptr || HammerRouteTimelineComponent == nullptr || HammerRouteCurve == nullptr || Owner == nullptr)
	{
		return;
	}
	
	const AObsidianPlayerState* OwningPS = Cast<AObsidianPlayerState>(Owner);
	if (OwningPS == nullptr)
	{
		return;
	}

	const AObsidianHero* HeroOwner = OwningPS->GetObsidianHero();
	if (HeroOwner == nullptr)
	{
		return;
	}

	HammerRouteSplineComponent = Cast<USplineComponent>(
		AddComponentByClass(USplineComponent::StaticClass(), true, FTransform::Identity, false)
		);
	if (HammerRouteSplineComponent == nullptr)
	{
		return;
	}
	
	const FVector StartingPosition = HeroOwner->GetActorLocation();
	DEBUG_SPHERE(StartingPosition);
	UObsidianBlueprintFunctionLibrary::PrintVector3D(this, StartingPosition, TEXT("Starting Location: "));
	const FVector MidWayPosition = StartingPosition + (HeroOwner->GetActorForwardVector() * HammerDistance);
	DEBUG_SPHERE(MidWayPosition);
	UObsidianBlueprintFunctionLibrary::PrintVector3D(this, MidWayPosition, TEXT("Midway Location: "));
	DEBUG_LINE(StartingPosition, MidWayPosition);
	const FVector FinishPosition = StartingPosition;
	UObsidianBlueprintFunctionLibrary::PrintVector3D(this, FinishPosition, TEXT("Finish Location: "));
	DEBUG_SPHERE(FinishPosition);
	const TArray<FVector> HammerRouteSplinePoints = {StartingPosition, MidWayPosition, FinishPosition};
	HammerRouteSplineComponent->SetSplinePoints(HammerRouteSplinePoints, ESplineCoordinateSpace::World, true);
	HammerRouteSplineComponent->UpdateSpline();

	for (int32 i = 0; i < HammerRouteSplineComponent->GetNumberOfSplinePoints(); ++i)
	{
		const FVector Point = HammerRouteSplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
		UE_LOG(LogTemp, Warning, TEXT("Spline Point %d: %s"), i, *Point.ToString());
	}

	FOnTimelineFloat ProgressUpdate;
	ProgressUpdate.BindDynamic(this, &ThisClass::UpdateHammerRoute);
	HammerRouteTimelineComponent->AddInterpFloat(HammerRouteCurve, ProgressUpdate);

	FOnTimelineEventStatic OnFinished;
	OnFinished.BindLambda([this]()
		{
			OnHammerRouteFinished();
		});

	HammerRouteTimelineComponent->SetTimelineFinishedFunc(OnFinished);
	HammerRouteTimelineComponent->PlayFromStart();
}

void AObsidianMagneticHammerProjectile::UpdateHammerRoute(float UpdateAlpha)
{
	UE_LOG(LogTemp, Warning, TEXT("Update Alpha: %f"), UpdateAlpha);
	const FVector NewLocation = HammerRouteSplineComponent->GetLocationAtTime(UpdateAlpha, ESplineCoordinateSpace::World);
	UObsidianBlueprintFunctionLibrary::PrintVector3D(this, NewLocation, TEXT("New Proj Location: "));
	SetActorLocationAndRotation(NewLocation, GetActorRotation());
}

void AObsidianMagneticHammerProjectile::OnHammerRouteFinished()
{
	Destroy();
}

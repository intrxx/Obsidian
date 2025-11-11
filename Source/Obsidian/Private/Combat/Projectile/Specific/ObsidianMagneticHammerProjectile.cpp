// Copyright 2024 out of sCope team - Michał Ogiński

#include "Combat/Projectile/Specific/ObsidianMagneticHammerProjectile.h"


#include "Characters/Heroes/ObsidianHero.h"
#include "Combat/Projectile/OProjectileMovementComponent.h"

AObsidianMagneticHammerProjectile::AObsidianMagneticHammerProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HammerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hammer Mesh"));
	HammerMeshComponent->SetupAttachment(GetRootComponent());
	HammerMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	HammerMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	HammerMeshComponent->SetGenerateOverlapEvents(false);
	
	ProjectileCleanupMethod = EObsidianProjectileCleanupMethod::Custom;
	bDestroyOnHit = false;
	bReturning = false;
	
	ProjectileMovementComponent->InitialSpeed = 500.0f;
	ProjectileMovementComponent->MaxSpeed = 500.0f;
	ProjectileMovementComponent->bIsHomingProjectile = true;
	ProjectileMovementComponent->HomingAccelerationMagnitude = 5000.0f;
}

void AObsidianMagneticHammerProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectileSpawnLocation = GetActorLocation();
	HammerMidwayLocation = ProjectileSpawnLocation + (GetActorForwardVector() * HammerRange);
	HammerMaxRotationSpeed = HammerRotationSpeed;
	HammerMaxSpeed = ProjectileMovementComponent->MaxSpeed;
	HeroOwner = Cast<AObsidianHero>(GetOwner());

	if (HammerMeshComponent)
	{
		HammerInitialScaleValue = HammerMeshComponent->GetRelativeScale3D();
	}
}

void AObsidianMagneticHammerProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateHammerRoute(DeltaTime);
}

void AObsidianMagneticHammerProjectile::UpdateHammerRoute(const float DeltaTime)
{
	if (HeroOwner.IsValid() == false)
	{
		FFrame::KismetExecutionMessage(TEXT("Projectile Owner Hero is no longer valid, destroying projectile. "),
			ELogVerbosity::Error);
		Destroy();
		return;
	}

	const AObsidianHero* HeroOwnerPtr = HeroOwner.Get();
	const FVector HammerFinalDestination = HeroOwnerPtr->GetActorLocation();
	const FVector CurrentHammerPosition = GetActorLocation();
	
	if (bReturning == false)
	{
		const float DistanceTraveled = FVector::Dist(ProjectileSpawnLocation, CurrentHammerPosition);
		if (DistanceTraveled > HammerRange)
		{
			bReturning = true;
			ProjectileMovementComponent->Velocity *= -1.0f;
			ProjectileMovementComponent->HomingTargetComponent = HeroOwnerPtr->GetRootComponent();
		}
		
		const float ForwardsDistanceTraveledPercent = FMath::Lerp(0.0f, 0.5f, DistanceTraveled / HammerRange);
		const float SpeedPercent = HammerRouteCurve->GetFloatValue(ForwardsDistanceTraveledPercent);
		HammerScaleValue = 2.0f - SpeedPercent;
		ProjectileMovementComponent->MaxSpeed = HammerMaxSpeed * SpeedPercent;
		HammerRotationSpeed = HammerMaxRotationSpeed * SpeedPercent;
	}
	else
	{
		const float DistanceToTravelBack = FVector::Dist(HammerMidwayLocation, HammerFinalDestination);
		const float DistanceTraveled = FVector::Dist(HammerMidwayLocation, CurrentHammerPosition);
		if (DistanceTraveled > DistanceToTravelBack)
		{
			Destroy();	
		}
		
		const float BackwardsTraveledDistancePercent = FMath::Lerp(0.5f, 1.0f, DistanceTraveled / (DistanceToTravelBack));
		const float SpeedPercent = HammerRouteCurve->GetFloatValue(BackwardsTraveledDistancePercent);
		HammerScaleValue = 2.0f - SpeedPercent;
		ProjectileMovementComponent->MaxSpeed = HammerMaxSpeed * SpeedPercent;
		HammerRotationSpeed = HammerMaxRotationSpeed * SpeedPercent;
	}
	
	if (HammerMeshComponent)
	{
		HammerMeshComponent->SetRelativeScale3D(HammerInitialScaleValue * FVector(HammerScaleValue));
		
		if (bReturning)
		{
			const FVector Direction = (HammerFinalDestination - CurrentHammerPosition).GetSafeNormal();
			const FRotator CurrentHammerRotation = GetActorRotation();
			const FRotator TargetRotation = FRotator(0.0f, Direction.Rotation().Yaw, 0.0f);
			const FRotator NewRotation = FMath::RInterpTo(CurrentHammerRotation, TargetRotation, DeltaTime, 5.0f);
			SetActorRotation(NewRotation);
		}
		
		HammerMeshComponent->AddLocalRotation(FRotator(HammerRotationSpeed * DeltaTime, 0.0f, 0.0f));
	}
}

void AObsidianMagneticHammerProjectile::OnHammerRouteFinished()
{
	Destroy();
}


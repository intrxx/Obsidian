// Copyright 2026 out of sCope team - intrxx

#include "Combat/Projectile/Specific/ObsidianMagneticHammerProjectile.h"


#include "Characters/Heroes/ObsidianHero.h"
#include "Combat/Projectile/OProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	}
	
	if (HammerMeshComponent)
	{
		if (bReturning)
		{
			const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CurrentHammerPosition, HammerFinalDestination);
			FRotator NewRotation = GetActorRotation();
			NewRotation.Yaw = TargetRotation.Yaw - 180.0f; // -180.0f cuz we are going backwards back to Owner
			SetActorRotation(NewRotation);
		}
		HammerMeshComponent->AddLocalRotation(FRotator(HammerRotationSpeed * DeltaTime, 0.0f, 0.0f));
		HammerMeshComponent->SetRelativeScale3D(HammerInitialScaleValue * FVector(HammerScaleValue));
	}
}

void AObsidianMagneticHammerProjectile::OnHammerRouteFinished()
{
	Destroy();
}


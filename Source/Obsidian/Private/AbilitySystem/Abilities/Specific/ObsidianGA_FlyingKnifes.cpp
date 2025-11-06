// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/Abilities/Specific/ObsidianGA_FlyingKnifes.h"


UObsidianGA_FlyingKnifes::UObsidianGA_FlyingKnifes(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UObsidianGA_FlyingKnifes::FireFlyingKnifeProjectiles()
{
	const FVector OwnerLocation = GetOwnerLocationFromActorInfo();
	if (OwnerLocation == FVector::ZeroVector)
	{
		UE_LOG(LogProjectileAbility, Warning, TEXT("OwnerLocation is ZeroVector in [%hs]"), ANSI_TO_TCHAR(__FUNCDNAME__));
	}

	const FVector SpawnPoint = FVector(OwnerLocation.X, OwnerLocation.Y, SpawnHeight);
	const TArray<FVector> RandomPoints = GetPointsOnCircleAroundOriginNormalized(OwnerLocation, NumberOfProjectiles, SpawnRadius, SpawnHeight);
	for (uint8 i = 0; i < RandomPoints.Num(); ++i)
	{
		SpawnProjectile(SpawnPoint, RandomPoints[i], false);
	}
}

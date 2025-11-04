// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/Abilities/Specific/ObGameplayAbility_FlyingKnife.h"


UObGameplayAbility_FlyingKnife::UObGameplayAbility_FlyingKnife(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UObGameplayAbility_FlyingKnife::FireFlyingKnifeProjectiles()
{
	const FVector OwnerLocation = GetOwnerLocationFromActorInfo();
	if (OwnerLocation == FVector::ZeroVector)
	{
		UE_LOG(LogProjectileAbility, Warning, TEXT("OwnerLocation is ZeroVector in [%hs]"), ANSI_TO_TCHAR(__FUNCDNAME__));
	}
	
	for (uint8 i = 0; i < NumberOfProjectiles; ++i)
	{
		const FVector RandomPoint = GetRandomPointInCircleAroundOrigin(OwnerLocation, SpawnRadius, SpawnHeight);
		const FVector ProjectileTargetLocation = FVector(RandomPoint.X * 5.0f, RandomPoint.Y * 5.0f, RandomPoint.Z);
		SpawnProjectile(RandomPoint, ProjectileTargetLocation, true);
	}
}

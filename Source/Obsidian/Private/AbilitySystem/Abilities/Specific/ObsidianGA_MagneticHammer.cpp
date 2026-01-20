// Copyright 2026 out of sCope team - intrxx


#include "AbilitySystem/Abilities/Specific/ObsidianGA_MagneticHammer.h"

UObsidianGA_MagneticHammer::UObsidianGA_MagneticHammer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UObsidianGA_MagneticHammer::FireMagneticHammer(const FVector& TowardsTarget)
{
	SpawnProjectile(GetOwnerLocationFromActorInfo(), TowardsTarget, true);
}

// Copyright 2026 out of sCope team - intrxx


#include "AbilitySystem/Abilities/Specific/ObsidianGA_MagneticHammer.h"


UObsidianGA_MagneticHammer::UObsidianGA_MagneticHammer(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

void UObsidianGA_MagneticHammer::FireMagneticHammer(const FVector& InTowardsTarget)
{
	SpawnProjectile(GetOwnerLocationFromActorInfo(), InTowardsTarget, true);
}

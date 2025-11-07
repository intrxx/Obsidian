// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Abilities/Specific/ObsidianGA_MagneticHammer.h"

UObsidianGA_MagneticHammer::UObsidianGA_MagneticHammer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UObsidianGA_MagneticHammer::FireMagneticHammer(const FVector& TowardsTarget)
{
	SpawnProjectile(GetOwnerLocationFromActorInfo(), TowardsTarget, true);
}

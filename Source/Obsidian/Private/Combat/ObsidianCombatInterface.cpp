// Copyright 2024 out of sCope team - Michał Ogiński


#include "Combat/ObsidianCombatInterface.h"

int32 IObsidianCombatInterface::GetCharacterLevel()
{
	return 1;
}

void IObsidianCombatInterface::SetMotionWarpingFacingTarget_Implementation(const FName MotionWarpName, const FVector& FacingTarget)
{
	
}

FVector IObsidianCombatInterface::GetAbilitySocketLocationFromLHWeapon()
{
	return FVector();
}

FVector IObsidianCombatInterface::GetAbilitySocketLocationFromRHWeapon()
{
	return FVector();
}

FVector IObsidianCombatInterface::GetAbilitySocketLocationFromLeftHand()
{
	return FVector();
}

FVector IObsidianCombatInterface::GetAbilitySocketLocationFromRightHand()
{
	return FVector();
}

FVector IObsidianCombatInterface::GetAbilityDefaultLocation()
{
	return FVector();
}


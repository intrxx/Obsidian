// Copyright 2026 out of sCope team - intrxx

#include "Combat/ObsidianCombatInterface.h"

// ~ Core
#include "GameplayTagContainer.h"

// ~ Project

uint8 IObsidianCombatInterface::GetCharacterLevel()
{
	return 0;
}

bool IObsidianCombatInterface::IsDeadOrDying_Implementation() const
{
	return false;
}

AActor* IObsidianCombatInterface::GetAvatarActor_Implementation()
{
	return nullptr;
}

UNiagaraSystem* IObsidianCombatInterface::GetBloodEffect_Implementation()
{
	return nullptr;
}

void IObsidianCombatInterface::SetMotionWarpingFacingTarget_Implementation(const FName MotionWarpName, const FVector& FacingTarget)
{
	
}

FVector IObsidianCombatInterface::GetAbilitySocketLocationForTag_Implementation(FGameplayTag Tag)
{
	return FVector();
}

FVector IObsidianCombatInterface::GetAbilitySocketLocationFromLHWeapon_Implementation()
{
	return FVector();
}

FVector IObsidianCombatInterface::GetAbilitySocketLocationFromRHWeapon_Implementation()
{
	return FVector();
}

FVector IObsidianCombatInterface::GetAbilitySocketLocationFromLeftHand_Implementation()
{
	return FVector();
}

FVector IObsidianCombatInterface::GetAbilitySocketLocationFromRightHand_Implementation()
{
	return FVector();
}

FVector IObsidianCombatInterface::GetAbilityBetweenHandsSocketLocation_Implementation()
{
	return FVector();
}



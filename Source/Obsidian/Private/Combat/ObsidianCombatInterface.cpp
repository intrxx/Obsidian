// Copyright 2024 out of sCope team - Michał Ogiński


#include "Combat/ObsidianCombatInterface.h"

#include "GameplayTagContainer.h"

int32 IObsidianCombatInterface::GetCharacterLevel()
{
	return 1;
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

FVector IObsidianCombatInterface::GetAbilitySocketLocationForTag_Implementation(const FGameplayTag& Tag)
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



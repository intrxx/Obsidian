// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Abilities/AI/ObsidianAIGameplayAbility_Melee.h"
#include "AI/ObsidianEnemyInterface.h"
#include "GameFramework/MovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UObsidianAIGameplayAbility_Melee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                       const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if(AvatarActor == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}

	CombatTargetActor = IObsidianEnemyInterface::Execute_GetCombatTarget(AvatarActor);
	
	if(bShouldStopMovement)
	{
		UMovementComponent* MoveComp = GetMovementCompFromActorInfo();
		MoveComp->Deactivate();
	}

	if(bShouldRotateToTarget)
	{
		const FVector TargetLocation = CombatTargetActor->GetActorLocation();
		const FVector AvatarLocation = AvatarActor->GetActorLocation();

		const FRotator LookAtTargetRotation = UKismetMathLibrary::FindLookAtRotation(AvatarLocation, TargetLocation);

		FRotator AvatarRotation = AvatarActor->GetActorRotation();
		AvatarRotation.Yaw = LookAtTargetRotation.Yaw;
		
		AvatarActor->SetActorRotation(AvatarRotation);
	}
}

void UObsidianAIGameplayAbility_Melee::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if(bShouldStopMovement)
	{
		UMovementComponent* MoveComp = GetMovementCompFromActorInfo();
		MoveComp->Activate();
	}
}

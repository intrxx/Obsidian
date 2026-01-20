// Copyright 2026 out of sCope team - intrxx


#include "AbilitySystem/Abilities/AI/ObsidianAIGameplayAbility_Melee.h"

// ~ Core
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"

// ~ Project
#include "AI/ObsidianEnemyInterface.h"

void UObsidianAIGameplayAbility_Melee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                       const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if(AvatarActor == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}

	OwningAIController = OwningAIController.Get() == nullptr ? GetAIControllerFromActorInfo() : OwningAIController.Get();
	if(OwningAIController == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}

	CombatTargetActor = IObsidianEnemyInterface::Execute_GetCombatTarget(AvatarActor);
	if(CombatTargetActor == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
	
	if(bShouldStopMovement)
	{
		if(UPathFollowingComponent* PathFollowingComp = OwningAIController->GetPathFollowingComponent())
		{
			PathFollowingComp->LockResource(RequestPriority);
		}
	}

	if(bShouldRotateToTarget && CombatTargetActor)
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
		if(UPathFollowingComponent* PathFollowingComp = OwningAIController->GetPathFollowingComponent())
		{
			PathFollowingComp->ClearResourceLock(RequestPriority);
		}
	}
}

AAIController* UObsidianAIGameplayAbility_Melee::GetAIControllerFromActorInfo()
{
	if(CurrentActorInfo == nullptr)
	{
		return nullptr;
	}
	
	if(APawn* Pawn = Cast<APawn>(CurrentActorInfo->AvatarActor.Get()))
	{
		return Cast<AAIController>(Pawn->GetController());
	}

	return nullptr;
}

// Copyright 2026 out of sCope team - intrxx


#include "AbilitySystem/Abilities/AI/ObsidianAIGameplayAbility_Melee.h"

#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"

#include "AI/ObsidianEnemyInterface.h"


void UObsidianAIGameplayAbility_Melee::ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo,
                                                       const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData)
{
	Super::ActivateAbility(InHandle, InActorInfo, InActivationInfo, InTriggerEventData);

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if(AvatarActor == nullptr)
	{
		CancelAbility(InHandle, InActorInfo, InActivationInfo, true);
	}

	OwningAIController = OwningAIController.Get() == nullptr ? GetAIControllerFromActorInfo() : OwningAIController.Get();
	if(OwningAIController == nullptr)
	{
		CancelAbility(InHandle, InActorInfo, InActivationInfo, true);
	}

	CombatTargetActor = IObsidianEnemyInterface::Execute_GetCombatTarget(AvatarActor);
	if(CombatTargetActor == nullptr)
	{
		CancelAbility(InHandle, InActorInfo, InActivationInfo, true);
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

void UObsidianAIGameplayAbility_Melee::EndAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo,
	const FGameplayAbilityActivationInfo InActivationInfo, bool bInReplicateEndAbility, bool bInWasCancelled)
{
	Super::EndAbility(InHandle, InActorInfo, InActivationInfo, bInReplicateEndAbility, bInWasCancelled);

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

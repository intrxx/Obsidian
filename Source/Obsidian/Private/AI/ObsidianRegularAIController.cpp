// Copyright 2026 out of sCope team - intrxx

#include "AI/ObsidianRegularAIController.h"

// ~ Core
#include "AbilitySystemGlobals.h"

// ~ Project
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Enemies/ObsidianRegularEnemy.h"

UObsidianAbilitySystemComponent* AObsidianRegularAIController::GetObsidianAbilitySystemComponent() const
{
	if(EnemyOwner)
	{
		return CastChecked<UObsidianAbilitySystemComponent>(EnemyOwner->GetAbilitySystemComponent());
	}

	if(UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn()))
	{
		return CastChecked<UObsidianAbilitySystemComponent>(ASC);
	}
	
	return nullptr;
}

void AObsidianRegularAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	EnemyOwner = Cast<AObsidianRegularEnemy>(InPawn);
}

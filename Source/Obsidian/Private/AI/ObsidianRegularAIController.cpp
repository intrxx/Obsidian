// Copyright 2026 out of sCope team - intrxx

#include "AI/ObsidianRegularAIController.h"

#include "AbilitySystemGlobals.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Enemies/ObsidianRegularEnemy.h"


AObsidianRegularAIController::AObsidianRegularAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComponent"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
}

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

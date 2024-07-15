// Copyright 2024 out of sCope team - Michał Ogiński


#include "AI/ObsidianAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AObsidianAIController::AObsidianAIController()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComponent"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));

	bStartAILogicOnPossess = true;
	bStopAILogicOnUnposses = true;
}

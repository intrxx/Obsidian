// Copyright 2024 out of sCope team - Michał Ogiński


#include "AI/AObsidianAIControllerBase.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AObsidianAIControllerBase::AObsidianAIControllerBase()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComponent"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));

	bStartAILogicOnPossess = true;
	bStopAILogicOnUnposses = true;
}

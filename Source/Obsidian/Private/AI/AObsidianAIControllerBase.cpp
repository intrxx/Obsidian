// Copyright 2026 out of sCope team - intrxx

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



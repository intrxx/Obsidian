// Copyright 2026 out of sCope team - intrxx

#include "AI/AObsidianAIControllerBase.h"

// ~ Core
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

// ~ Project

AObsidianAIControllerBase::AObsidianAIControllerBase()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComponent"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));

	bStartAILogicOnPossess = true;
	bStopAILogicOnUnposses = true;
	
}



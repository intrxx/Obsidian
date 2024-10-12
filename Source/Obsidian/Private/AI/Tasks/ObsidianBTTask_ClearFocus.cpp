// Copyright 2024 out of sCope team - Michał Ogiński


#include "AI/Tasks/ObsidianBTTask_ClearFocus.h"
#include "AIController.h"

UObsidianBTTask_ClearFocus::UObsidianBTTask_ClearFocus()
{
	NodeName = FString("Clear Focus");
	INIT_TASK_NODE_NOTIFY_FLAGS();
}

EBTNodeResult::Type UObsidianBTTask_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(AAIController* AIController = OwnerComp.GetAIOwner())
	{
		AIController->ClearFocus(EAIFocusPriority::Gameplay);

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

FString UObsidianBTTask_ClearFocus::GetStaticDescription() const
{
	return FString::Printf(TEXT("Cleares Focus from AI Controller."));
}

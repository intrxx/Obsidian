// Copyright 2026 out of sCope team - intrxx

#include "AI/Tasks/ObsidianBTTask_SetFocus.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


UObsidianBTTask_SetFocus::UObsidianBTTask_SetFocus()
{
	NodeName = FString("Set Focus");
	INIT_TASK_NODE_NOTIFY_FLAGS();
}

EBTNodeResult::Type UObsidianBTTask_SetFocus::ExecuteTask(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

	AAIController* AIController = InOwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = InOwnerComp.GetBlackboardComponent();
	if(AIController && BlackboardComponent)
	{
		if(AActor* Target = Cast<AActor>(BlackboardComponent->GetValueAsObject(Target_Selector.SelectedKeyName)))
		{
			AIController->SetFocus(Target);
			
			NodeResult = EBTNodeResult::Succeeded;
		}
		else
		{
			UE_VLOG(InOwnerComp.GetOwner(), LogBehaviorTree, Error, TEXT("UObsidianBTTask_SetFocus::ExecuteTask failed since Target to Focus is missing."));
			NodeResult = EBTNodeResult::Failed;
		}
	}
	else
	{
		UE_VLOG(InOwnerComp.GetOwner(), LogBehaviorTree, Error, TEXT("UObsidianBTTask_SetFocus::ExecuteTask failed since AIController is missing."));
		NodeResult = EBTNodeResult::Failed;
	}
	
	return NodeResult;
}

FString UObsidianBTTask_SetFocus::GetStaticDescription() const
{
	return FString::Printf(TEXT("Sets Focus on Target's AI Controller. \n")) +=
		FString::Printf(TEXT("Target Key: [%s] \n"), *Target_Selector.SelectedKeyName.ToString());
}

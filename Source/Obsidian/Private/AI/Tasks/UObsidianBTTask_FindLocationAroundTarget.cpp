// Copyright 2024 out of sCope team - Michał Ogiński


#include "AI/Tasks/UObsidianBTTask_FindLocationAroundTarget.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UObsidianBTTask_FindLocationAroundTarget::UObsidianBTTask_FindLocationAroundTarget()
{
	NodeName = FString("Find Location Around Target");
	INIT_TASK_NODE_NOTIFY_FLAGS();
}

EBTNodeResult::Type UObsidianBTTask_FindLocationAroundTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if(AIController == nullptr)
	{
		UE_VLOG(OwnerComp.GetOwner(), LogBehaviorTree, Error, TEXT("UObsidianBTTask_UseGameplayAbility::ExecuteTask failed since AIController is missing."));
		NodeResult = EBTNodeResult::Failed;
	}
	else
	{
		NodeResult = PerformFindLocationAroundTargetTask(OwnerComp, NodeMemory);
	}
	
	return NodeResult;
}

EBTNodeResult::Type UObsidianBTTask_FindLocationAroundTarget::PerformFindLocationAroundTargetTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;
	
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	
	if(const AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(Target_Selector.SelectedKeyName)))
	{
		const FVector Origin = TargetActor->GetActorLocation();
		FVector NewLocation = Origin;
		FNavLocation RandomPoint(Origin);
		bool bSuccess = false;
		
		UWorld* World = GetWorld();
		
		if(UNavigationSystemV1* NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
		{
			if(ANavigationData* NavigationData = NavigationSystem->GetDefaultNavDataInstance(FNavigationSystem::DontCreate))
			{
				if(NavigationSystem->GetRandomPointInNavigableRadius(Origin, RadiusAroundTheTarget, RandomPoint, NavigationData))
				{
					NewLocation = RandomPoint.Location;
					
					BlackboardComponent->SetValueAsVector(NewLocationAroundTarget_Selector.SelectedKeyName, NewLocation);
					NodeResult = EBTNodeResult::Succeeded;
					bSuccess = true;
				}
			}
		}

		if(bFallBackToDefaultIfFailed && !bSuccess)
		{
			BlackboardComponent->SetValueAsVector(NewLocationAroundTarget_Selector.SelectedKeyName, NewLocation);
		}
	}
	
	return NodeResult;
}

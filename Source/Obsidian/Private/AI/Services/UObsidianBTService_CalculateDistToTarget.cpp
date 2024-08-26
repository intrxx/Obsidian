// Copyright 2024 out of sCope team - Michał Ogiński


#include "AI/Services/UObsidianBTService_CalculateDistToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UObsidianBTService_CalculateDistToTarget::UObsidianBTService_CalculateDistToTarget()
{
	NodeName = FString("Calculate Distance to Target");
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
	
	Interval = 0.3f;
	RandomDeviation = 0.0f;
}

void UObsidianBTService_CalculateDistToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const AAIController* AIController = OwnerComp.GetAIOwner();
	const APawn* OwningPawn = AIController->GetPawn();
	
	if(!IsValid(OwningPawn))
	{
		return;
	}

	UBlackboardComponent* OwnerBlackBoardComp = OwnerComp.GetBlackboardComponent();

	const AActor* TargetActor = Cast<AActor>(OwnerBlackBoardComp->GetValueAsObject(TargetActor_Selector.SelectedKeyName));
	const float DistanceToActor = OwningPawn->GetDistanceTo(TargetActor);

	OwnerBlackBoardComp->SetValueAsFloat(DistanceToTargetActor_Selector.SelectedKeyName, DistanceToActor);
}

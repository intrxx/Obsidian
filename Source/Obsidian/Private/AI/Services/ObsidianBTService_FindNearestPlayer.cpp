// Copyright 2024 out of sCope team - Michał Ogiński


#include "AI/Services/ObsidianBTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ObsidianTypes/ObsidianActorTags.h"

UObsidianBTService_FindNearestPlayer::UObsidianBTService_FindNearestPlayer()
{
	NodeName = FString("Find Nearest Player");
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
	
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UObsidianBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const AAIController* AIController = OwnerComp.GetAIOwner();
	const APawn* OwningPawn = AIController->GetPawn();
	
	if(!IsValid(OwningPawn))
	{
		return;
	}
	
	const FName TargetTag = OwningPawn->ActorHasTag(ObsidianActorTags::Player) ? ObsidianActorTags::Enemy : ObsidianActorTags::Player;

	TArray<AActor*> TargetActors;
	UGameplayStatics::GetAllActorsWithTag(this, TargetTag, TargetActors);

	float NearestDistance = UE_FLOAT_HUGE_DISTANCE;
	AActor* NearestActor = nullptr;
	
	for(AActor* Actor : TargetActors)
	{
		const float DistanceToTarget = OwningPawn->GetDistanceTo(Actor);
		if(DistanceToTarget < NearestDistance)
		{
			NearestDistance = DistanceToTarget;
			NearestActor = Actor;
		}
	}

	UBlackboardComponent* OwnerBlackBoardComp = OwnerComp.GetBlackboardComponent();
	OwnerBlackBoardComp->SetValueAsObject(NearestTargetActor_Selector.SelectedKeyName, NearestActor);
	OwnerBlackBoardComp->SetValueAsFloat(DistanceToTargetActor_Selector.SelectedKeyName, NearestDistance);
}

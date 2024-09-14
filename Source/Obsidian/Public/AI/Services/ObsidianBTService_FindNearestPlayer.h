// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "ObsidianBTService_FindNearestPlayer.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianBTService_FindNearestPlayer : public UBTService
{
	GENERATED_BODY()

public:
	UObsidianBTService_FindNearestPlayer();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "Nearest Target Actor"), Category = "Key")
	FBlackboardKeySelector NearestTargetActor_Selector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "Distance To Target Actor"), Category = "Key")
	FBlackboardKeySelector DistanceToTargetActor_Selector;
};

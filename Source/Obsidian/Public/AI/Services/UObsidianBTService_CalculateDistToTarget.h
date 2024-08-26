// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "UObsidianBTService_CalculateDistToTarget.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianBTService_CalculateDistToTarget : public UBTService
{
	GENERATED_BODY()

public:
	UObsidianBTService_CalculateDistToTarget();
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	/** Target Actor to calculate the distance to. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian")
	FBlackboardKeySelector TargetActor_Selector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian")
	FBlackboardKeySelector DistanceToTargetActor_Selector;
};

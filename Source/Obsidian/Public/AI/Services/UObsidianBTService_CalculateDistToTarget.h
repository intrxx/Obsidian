// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "BehaviorTree/BTService.h"
#include "CoreMinimal.h"

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
	virtual void TickNode(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory, float InDeltaSeconds) override;
	virtual FString GetStaticDescription() const override;
	
protected:
	/** Target Actor to calculate the distance to. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "Target Actor"), Category = "Key")
	FBlackboardKeySelector TargetActor_Selector;

	/** Distance to the selected Actor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "Distance To Target"), Category = "Key")
	FBlackboardKeySelector DistanceToTargetActor_Selector;
};

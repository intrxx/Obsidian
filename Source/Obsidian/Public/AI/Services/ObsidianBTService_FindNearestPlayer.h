// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "CoreMinimal.h"

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
	virtual void TickNode(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory, float InDeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

protected:
	/** Nearest Target Actor Key returned by the Service. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "Nearest Target Actor"), Category = "Key")
	FBlackboardKeySelector NearestTargetActor_Selector;

	/** Distance to the nearest Actor returned by the Service. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "Distance To Target Actor"), Category = "Key")
	FBlackboardKeySelector DistanceToTargetActor_Selector;
};

// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


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
	virtual FString GetStaticDescription() const override;

protected:
	/** Nearest Target Actor Key returned by the Service. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "Nearest Target Actor"), Category = "Key")
	FBlackboardKeySelector NearestTargetActor_Selector;

	/** Distance to the nearest Actor returned by the Service. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "Distance To Target Actor"), Category = "Key")
	FBlackboardKeySelector DistanceToTargetActor_Selector;
};

// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "CoreMinimal.h"

#include "ObsidianBTService_FindNearestPlayer.generated.h"

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Key"))
class OBSIDIAN_API UObsidianBTService_FindNearestPlayer : public UBTService
{
	GENERATED_BODY()

public:
	UObsidianBTService_FindNearestPlayer();

	//~ Begin UBTNode Interface
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface

protected:
	//~ Begin UBTAuxiliaryNode Interface
	virtual void TickNode(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory, float InDeltaSeconds) override;
	//~ End UBTAuxiliaryNode Interface

	/** Nearest Target Actor Key returned by the Service. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "Nearest Target Actor"), Category = "Key")
	FBlackboardKeySelector NearestTargetActor_Selector;

	/** Distance to the nearest Actor returned by the Service. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "Distance To Target Actor"), Category = "Key")
	FBlackboardKeySelector DistanceToTargetActor_Selector;
};

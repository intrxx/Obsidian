// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "CoreMinimal.h"

#include "ObsidianBTTask_FindLocationAroundTarget.generated.h"

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Key Obsidian"))
class OBSIDIAN_API UObsidianBTTask_FindLocationAroundTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	//~ Begin UBTNode Interface
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface

	//~ Begin UBTTaskNode Interface
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory) override;
	//~ End UBTTaskNode Interface

protected:
	virtual EBTNodeResult::Type PerformFindLocationAroundTargetTask(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "New Location Around Target"), Category = "Key")
	FBlackboardKeySelector NewLocationAroundTarget_Selector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "Target"), Category = "Key")
	FBlackboardKeySelector Target_Selector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian")
	float RadiusAroundTheTarget = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian")
	bool bFallBackToDefaultIfFailed = false;

private:
	UObsidianBTTask_FindLocationAroundTarget();
};

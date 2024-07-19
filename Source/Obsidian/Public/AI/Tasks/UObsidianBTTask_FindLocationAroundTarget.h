// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UObsidianBTTask_FindLocationAroundTarget.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianBTTask_FindLocationAroundTarget : public UBTTaskNode
{
	GENERATED_BODY()

	UObsidianBTTask_FindLocationAroundTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual EBTNodeResult::Type PerformFindLocationAroundTargetTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian")
	FBlackboardKeySelector NewLocationAroundTarget_Selector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian")
	FBlackboardKeySelector Target_Selector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian")
	float RadiusAroundTheTarget = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian")
	bool bFallBackToDefaultIfFailed = false;
};

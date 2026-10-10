// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "CoreMinimal.h"

#include "ObsidianBTTask_ClearFocus.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianBTTask_ClearFocus : public UBTTaskNode
{
	GENERATED_BODY()

	UObsidianBTTask_ClearFocus();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual FString GetStaticDescription() const override;
};

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "BehaviorTree/BTTaskNode.h"
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

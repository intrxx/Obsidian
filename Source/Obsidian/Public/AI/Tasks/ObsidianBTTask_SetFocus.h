// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "BehaviorTree/BTTaskNode.h"
#include "ObsidianBTTask_SetFocus.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianBTTask_SetFocus : public UBTTaskNode
{
	GENERATED_BODY()
	
	UObsidianBTTask_SetFocus();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "Focus Target"), Category = "Key")
	FBlackboardKeySelector Target_Selector;
};

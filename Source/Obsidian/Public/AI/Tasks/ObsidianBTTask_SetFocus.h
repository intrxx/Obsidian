// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "CoreMinimal.h"

#include "ObsidianBTTask_SetFocus.generated.h"

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Key"))
class OBSIDIAN_API UObsidianBTTask_SetFocus : public UBTTaskNode
{
	GENERATED_BODY()

public:
	//~ Begin UBTTaskNode Interface
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory) override;
	//~ End UBTTaskNode Interface

	//~ Begin UBTNode Interface
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "Focus Target"), Category = "Key")
	FBlackboardKeySelector Target_Selector;

private:
	UObsidianBTTask_SetFocus();
};

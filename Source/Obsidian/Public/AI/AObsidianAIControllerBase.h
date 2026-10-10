// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "AIController.h"
#include "CoreMinimal.h"

#include "AObsidianAIControllerBase.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	AObsidianAIControllerBase();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Enemy", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComp;
};

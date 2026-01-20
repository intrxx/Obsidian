// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "AIController.h"
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

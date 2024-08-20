// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
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

// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "AI/AObsidianAIControllerBase.h"

#include "ObsidianRegularAIController.generated.h"

class AObsidianRegularEnemy;
class UObsidianAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianRegularAIController : public AObsidianAIControllerBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerController")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const;

protected:
	//~ Begin AController Interface
	virtual void OnPossess(APawn* InPawn) override;
	//~ End AController Interface

	UPROPERTY()
	TObjectPtr<AObsidianRegularEnemy> EnemyOwner;
};

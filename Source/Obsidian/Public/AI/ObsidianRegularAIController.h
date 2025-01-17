// Copyright 2024 out of sCope team - Michał Ogiński

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
	virtual void OnPossess(APawn* InPawn) override;
	
protected:
	UPROPERTY()
	TObjectPtr<AObsidianRegularEnemy> EnemyOwner;
};

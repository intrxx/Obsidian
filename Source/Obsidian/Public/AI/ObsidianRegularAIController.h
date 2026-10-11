// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "AI/AObsidianAIControllerBase.h"

#include "ObsidianRegularAIController.generated.h"

class AObsidianRegularEnemy;
class UBehaviorTreeComponent;
class UObsidianAbilitySystemComponent;

/**
 * AI Controller of the Regular Enemies, runs their Behavior Tree.
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianRegularAIController : public AObsidianAIControllerBase
{
	GENERATED_BODY()

public:
	AObsidianRegularAIController();

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerController")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const;

protected:
	//~ Begin AController Interface
	virtual void OnPossess(APawn* InPawn) override;
	//~ End AController Interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Enemy", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComp;

	UPROPERTY()
	TObjectPtr<AObsidianRegularEnemy> EnemyOwner;
};

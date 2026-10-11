// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "AObsidianAIControllerBase.h"

#include "ObsidianBossAIController.generated.h"

class AObsidianBossEnemy;
class UObsidianAbilitySystemComponent;
struct FAIStimulus;

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianBossAIController : public AObsidianAIControllerBase
{
	GENERATED_BODY()

public:
	AObsidianBossAIController();

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerController")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const;

protected:
	UFUNCTION()
	void OnStimulusChanged(AActor* InUpdatedActor, FAIStimulus InStimulus);

	//~ Begin AController Interface
	virtual void OnPossess(APawn* InPawn) override;
	//~ End AController Interface

	UPROPERTY()
	TObjectPtr<AObsidianBossEnemy> EnemyOwner;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Enemy", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAIPerceptionComponent> PerceptionComp;
};

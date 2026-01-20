// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "AObsidianAIControllerBase.h"
#include "ObsidianBossAIController.generated.h"

class AObsidianBossEnemy;
class UObsidianAbilitySystemComponent;
struct FAIStimulus;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianBossAIController : public AObsidianAIControllerBase
{
	GENERATED_BODY()
	
public:
	AObsidianBossAIController();

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerController")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const;

protected:
	UPROPERTY()
	TObjectPtr<AObsidianBossEnemy> EnemyOwner;
	
protected:
	UFUNCTION()
	void OnStimulusChanged(AActor* UpdatedActor, FAIStimulus Stimulus);

	virtual void OnPossess(APawn* InPawn) override;
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Enemy", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAIPerceptionComponent> PerceptionComp;
};

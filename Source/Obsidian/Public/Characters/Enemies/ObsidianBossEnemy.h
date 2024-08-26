// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Characters/Enemies/ObsidianEnemy.h"
#include "ObsidianBossEnemy.generated.h"

class AObsidianHero;
class UBlackboardComponent;
class AObsidianAIControllerBase;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianBossEnemy : public AObsidianEnemy
{
	GENERATED_BODY()

public:
	AObsidianBossEnemy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void PossessedBy(AController* NewController) override;
	
	/** Handles the introduction to the Player, plays anim montages or activates sound cues */
	void HandleIntroduction(UBlackboardComponent* OwningBlackboard, AObsidianHero* TargetActor);

protected:
	//~ Start of AObsidianCharacterBase
	virtual void OnAbilitySystemInitialized() override;
	
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor) override;

	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor) override;
	//~ End of AObsidianCharacterBase

private:
	UPROPERTY()
	TObjectPtr<AObsidianAIControllerBase> ObsidianBossAIController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = true), Category = "Obsidian|Introduction")
	TObjectPtr<UAnimMontage> IntroductionMontage;

	FTimerHandle IntroductionFinishedTimerHandle;
};

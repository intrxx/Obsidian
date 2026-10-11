// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "AObsidianAIControllerBase.h"

#include "ObsidianBossAIController.generated.h"

struct FAIStimulus;

class AObsidianBossEnemy;
class UObsidianAbilitySystemComponent;
class UStateTree;
class UStateTreeAIComponent;

/**
 * AI Controller of the Boss Enemies, runs their State Tree and provides it with the perceived Hero.
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianBossAIController : public AObsidianAIControllerBase
{
	GENERATED_BODY()

public:
	AObsidianBossAIController();

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerController")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const;

	/** Sets the State Tree the Controller runs, needs to be done before the AI Logic is started. */
	void SetStateTree(UStateTree* InStateTree);

	/** Sends the event to the running State Tree, used for things the Boss needs to react to right away. */
	void SendStateTreeEvent(const FGameplayTag& InEventTag);

	UStateTreeAIComponent* GetStateTreeComponent() const { return StateTreeComp; }
	AActor* GetHeroTarget() const { return HeroTarget; }
	bool IsPlayerSeen() const { return bIsPlayerSeen; }

protected:
	UFUNCTION()
	void OnStimulusChanged(AActor* InUpdatedActor, FAIStimulus InStimulus);

	//~ Begin AController Interface
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	//~ End AController Interface

	UPROPERTY()
	TObjectPtr<AObsidianBossEnemy> EnemyOwner;

	/** Hero that is currently seen by the Boss, the State Tree binds to it. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Obsidian|Enemy")
	TObjectPtr<AActor> HeroTarget;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Obsidian|Enemy")
	bool bIsPlayerSeen = false;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Enemy", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStateTreeAIComponent> StateTreeComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Enemy", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAIPerceptionComponent> PerceptionComp;
};

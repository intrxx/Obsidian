// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "Characters/Enemies/ObsidianEnemy.h"

#include "ObsidianRegularEnemy.generated.h"

class AObsidianAIControllerBase;
class UBehaviorTree;

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianRegularEnemy : public AObsidianEnemy
{
	GENERATED_BODY()

public:
	AObsidianRegularEnemy(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	void HitReactTagChanged(const FGameplayTag InCallbackTag, int32 InNewCount);

	AObsidianAIControllerBase* GetObsidianAIController() const { return ObsidianRegularAIController; }
	UBehaviorTree* GetDefaultBehaviorTree() const { return DefaultBehaviorTree; }

	//~ Begin APawn Interface
	virtual void PossessedBy(AController* InNewController) override;
	//~ End APawn Interface

protected:
	//~ Begin AObsidianCharacterBase Interface
	virtual void OnAbilitySystemInitialized() override;

	UFUNCTION()
	virtual void OnDeathStarted(AActor* InOwningActor) override;

	UFUNCTION()
	virtual void OnDeathFinished(AActor* InOwningActor) override;
	//~ End AObsidianCharacterBase Interface

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Obsidian|AI")
	TObjectPtr<UBehaviorTree> DefaultBehaviorTree;

private:
	void CreateHealthBarWidget() const;

	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> HealthBarWidgetComp;

	UPROPERTY()
	TObjectPtr<AObsidianAIControllerBase> ObsidianRegularAIController;
};

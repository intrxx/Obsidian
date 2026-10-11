// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "Characters/Enemies/ObsidianEnemy.h"

#include "ObsidianBossEnemy.generated.h"

class AObsidianBossAIController;
class AObsidianHero;
class UObsidianAdvancedCombatComponent;
class UObsidianBossComponent;
class UStateTree;

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianBossEnemy : public AObsidianEnemy
{
	GENERATED_BODY()

public:
	AObsidianBossEnemy(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "Handle Advanced Combat Hit"), Category = "Obsidian|Boss")
	void BP_HandleAdvancedCombatHit(const FHitResult& InHitResult);

	UStateTree* GetDefaultStateTree() const { return DefaultStateTree; }

	//~ Begin APawn Interface
	virtual void PossessedBy(AController* InNewController) override;
	//~ End APawn Interface

protected:
	UFUNCTION()
	void HandleAdvancedCombatHit(const FHitResult& InHitResult);

	//~ Begin AObsidianCharacterBase Interface
	virtual void OnAbilitySystemInitialized() override;

	UFUNCTION()
	virtual void OnDeathStarted(AActor* InOwningActor) override;

	UFUNCTION()
	virtual void OnDeathFinished(AActor* InOwningActor) override;
	//~ End AObsidianCharacterBase Interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianBossComponent> BossComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianAdvancedCombatComponent> AdvancedCombatComponent;

	/** State Tree that drives the Boss, run by its AI Controller. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Obsidian|AI")
	TObjectPtr<UStateTree> DefaultStateTree;

	UPROPERTY()
	TObjectPtr<AObsidianBossAIController> ObsidianBossAIController;
};

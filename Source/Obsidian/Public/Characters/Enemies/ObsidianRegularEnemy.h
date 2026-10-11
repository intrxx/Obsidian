// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "MassEntityHandle.h"

#include "Characters/Enemies/ObsidianEnemy.h"

#include "ObsidianRegularEnemy.generated.h"

struct FObsidianMassEnemyIdentityFragment;

class AObsidianAIControllerBase;
class UBehaviorTree;
class UMassEntityConfigAsset;

/**
 * Enemy that comes in numbers. It is driven either by the Behavior Tree run by its AI Controller, or - if it has the
 * Mass Entity Config set - by a Mass entity. Entity is what lives in the World, it is hydrated into this Actor
 * (with the Ability System and everything else the combat needs) only when a Player is close enough.
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianRegularEnemy : public AObsidianEnemy
{
	GENERATED_BODY()

public:
	AObsidianRegularEnemy(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	void HitReactTagChanged(const FGameplayTag InCallbackTag, int32 InNewCount);

	/**
	 * Called by Mass right before the Enemy is spawned for the entity it is hydrated from, so before it begins play.
	 * Gives the Enemy everything that needs to survive the entity being dehydrated.
	 */
	void InitializeFromMassEntity(const FMassEntityHandle InEntity, const FObsidianMassEnemyIdentityFragment& InIdentity);

	AObsidianAIControllerBase* GetObsidianAIController() const { return ObsidianRegularAIController; }
	UBehaviorTree* GetDefaultBehaviorTree() const { return DefaultBehaviorTree; }
	UMassEntityConfigAsset* GetMassEntityConfig() const { return MassEntityConfig; }

	//~ Begin AObsidianEnemy Interface
	virtual bool IsDrivenByMass() const override { return MassEntityConfig != nullptr; }
	//~ End AObsidianEnemy Interface

	//~ Begin AActor Interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	//~ End AActor Interface

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

	virtual void DestroyDeadCharacter() override;
	//~ End AObsidianCharacterBase Interface

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Obsidian|AI")
	TObjectPtr<UBehaviorTree> DefaultBehaviorTree;

	/**
	 * Setting this makes the Enemy driven by Mass instead of the Behavior Tree. Enemies that are placed in the World
	 * or spawned by hand turn themselves into the entities of this config when they begin play.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Obsidian|AI")
	TObjectPtr<UMassEntityConfigAsset> MassEntityConfig;

private:
	void CreateHealthBarWidget() const;

	/** Replaces the Enemy with a new entity that has its transform and identity, returns false if it could not be created. */
	bool ConvertToMassEntity();

	/** Lets the State Tree of the entity know about what happens to the Actor - hit reacts and death. */
	void UpdateMassEnemyState();

	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> HealthBarWidgetComp;

	UPROPERTY()
	TObjectPtr<AObsidianAIControllerBase> ObsidianRegularAIController;

	/** Entity the Enemy was hydrated from, not set for the Enemies that are not spawned by Mass. */
	FMassEntityHandle MassEntity;
};

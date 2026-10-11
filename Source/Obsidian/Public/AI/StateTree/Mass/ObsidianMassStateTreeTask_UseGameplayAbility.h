// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MassStateTreeTypes.h"

#include "ObsidianMassStateTreeTask_UseGameplayAbility.generated.h"

struct FMassActorFragment;
struct FObsidianMassCombatTargetFragment;

/**
 * Every Task needs to have instance data, this one has nothing to keep in it.
 */
USTRUCT()
struct FObsidianMassStateTreeTask_UseGameplayAbilityInstanceData
{
	GENERATED_BODY()
};

/**
 * Task for activating the Gameplay Ability on the Actor the Enemy entity is hydrated into. The State Trees of the
 * entities are not guaranteed to run on the game thread, so the activation is deferred to the end of the Mass
 * processing phase. Succeeds right after the activation was requested, it does not wait for the Ability to end.
 */
USTRUCT(meta = (DisplayName = "Use Gameplay Ability (Mass)", Category = "Obsidian"))
struct OBSIDIAN_API FObsidianMassStateTreeTask_UseGameplayAbility : public FMassStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FObsidianMassStateTreeTask_UseGameplayAbilityInstanceData;

public:
	FObsidianMassStateTreeTask_UseGameplayAbility();

	//~ Begin FStateTreeNodeBase Interface
	virtual bool Link(FStateTreeLinker& InLinker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	//~ End FStateTreeNodeBase Interface

	//~ Begin FStateTreeTaskBase Interface
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const override;
	//~ End FStateTreeTaskBase Interface

	/** Ability tag used for activation. */
	UPROPERTY(EditAnywhere, meta = (Categories = "AbilityActivation"), Category = "Obsidian")
	FGameplayTagContainer ActivateAbilityWithTag;

	/** Should this task set the Target of the entity as the Combat Target on EnemyInterface. */
	UPROPERTY(EditAnywhere, Category = "Obsidian|CombatTarget")
	bool bSetCombatTargetOnEnemyInterface = false;

private:
	TStateTreeExternalDataHandle<FMassActorFragment> ActorHandle;
	TStateTreeExternalDataHandle<FObsidianMassCombatTargetFragment> CombatTargetHandle;
};

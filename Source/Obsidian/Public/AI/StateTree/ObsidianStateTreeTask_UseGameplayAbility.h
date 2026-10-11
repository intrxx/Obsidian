// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StateTreeTaskBase.h"

#include "ObsidianStateTreeTask_UseGameplayAbility.generated.h"

/**
 *
 */
USTRUCT()
struct FObsidianStateTreeTask_UseGameplayAbilityInstanceData
{
	GENERATED_BODY()

	/** Actor that owns the Ability System the Ability is activated on. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> Actor = nullptr;

	/** Combat Target to set on EnemyInterface. Make sure to bind this or the Combat Target will be nullptr! */
	UPROPERTY(EditAnywhere, Category = "Input", meta = (Optional))
	TObjectPtr<AActor> CombatTarget = nullptr;
};

/**
 * Task for activating the Gameplay Ability on the State Tree Owner. Succeeds right after the activation was tried,
 * it does not wait for the Ability to end.
 */
USTRUCT(meta = (DisplayName = "Use Gameplay Ability", Category = "Obsidian"))
struct OBSIDIAN_API FObsidianStateTreeTask_UseGameplayAbility : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FObsidianStateTreeTask_UseGameplayAbilityInstanceData;

public:
	FObsidianStateTreeTask_UseGameplayAbility();

	//~ Begin FStateTreeNodeBase Interface
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& InID, FStateTreeDataView InInstanceDataView, const IStateTreeBindingLookup& InBindingLookup,
		EStateTreeNodeFormatting InFormatting = EStateTreeNodeFormatting::Text) const override;
#endif
	//~ End FStateTreeNodeBase Interface

	//~ Begin FStateTreeTaskBase Interface
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const override;
	//~ End FStateTreeTaskBase Interface

	/** Ability tag used for activation. */
	UPROPERTY(EditAnywhere, meta = (Categories = "AbilityActivation"), Category = "Obsidian")
	FGameplayTagContainer ActivateAbilityWithTag;

	/** Should this task set the Combat Target on EnemyInterface. */
	UPROPERTY(EditAnywhere, Category = "Obsidian|CombatTarget")
	bool bSetCombatTargetOnEnemyInterface = false;

	UPROPERTY(EditAnywhere, Category = "Obsidian")
	bool bDebugEnabled = false;
};

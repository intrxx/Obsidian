// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StateTreeConditionBase.h"

#include "ObsidianStateTreeCondition_CanActivateAbility.generated.h"

/**
 *
 */
USTRUCT()
struct FObsidianStateTreeCondition_CanActivateAbilityInstanceData
{
	GENERATED_BODY()

	/** Actor that owns the Ability, needs to be an Obsidian Character. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> Actor = nullptr;

	/** Enemy Target, used to get tags from it. */
	UPROPERTY(EditAnywhere, Category = "Input", meta = (Optional))
	TObjectPtr<AActor> TargetActor = nullptr;
};

/**
 * Condition that passes if the State Tree Owner is able to activate the Ability with given tag right now.
 */
USTRUCT(meta = (DisplayName = "Can Activate Ability", Category = "Obsidian"))
struct OBSIDIAN_API FObsidianStateTreeCondition_CanActivateAbility : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FObsidianStateTreeCondition_CanActivateAbilityInstanceData;

public:
	//~ Begin FStateTreeNodeBase Interface
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& InID, FStateTreeDataView InInstanceDataView, const IStateTreeBindingLookup& InBindingLookup,
		EStateTreeNodeFormatting InFormatting = EStateTreeNodeFormatting::Text) const override;
#endif
	//~ End FStateTreeNodeBase Interface

	//~ Begin FStateTreeConditionBase Interface
	virtual bool TestCondition(FStateTreeExecutionContext& InContext) const override;
	//~ End FStateTreeConditionBase Interface

	/** Ability tag to check the activation for. */
	UPROPERTY(EditAnywhere, meta = (Categories = "AbilityActivation"), Category = "Obsidian")
	FGameplayTag ActivateAbilityWithTag = FGameplayTag();

	UPROPERTY(EditAnywhere, Category = "Obsidian")
	bool bDebugEnabled = false;
};

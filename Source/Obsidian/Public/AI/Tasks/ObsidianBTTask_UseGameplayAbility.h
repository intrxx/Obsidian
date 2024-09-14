// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ObsidianBTTask_UseGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianBTTask_UseGameplayAbility : public UBTTaskNode
{
	GENERATED_BODY()
	
	UObsidianBTTask_UseGameplayAbility();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual EBTNodeResult::Type PerformUseGameplayAbilityTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	/** Ability tag used for activation. */
	UPROPERTY(EditAnywhere, Category = "Obsidian")
	FGameplayTagContainer ActivateAbilityWithTag;

	/** Combat Target to set on EnemyInterface. Make sure to set this or the Combat Target will be nullptr! */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bSetCombatTargetOnEnemyInterface == true", EditConditionHides, DisplayName = "Combat Target"), Category = "Key")
	FBlackboardKeySelector CombatTarget_Selector;

	/** Should this task set the Combat Target on EnemyInterface. */
	UPROPERTY(EditAnywhere, Category = "Obsidian|CombatTarget")
	bool bSetCombatTargetOnEnemyInterface = false;
};

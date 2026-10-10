// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

#include "ObsidianBTTask_UseGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianBTTask_UseGameplayAbility : public UBTTaskNode
{
	GENERATED_BODY()
	
	UObsidianBTTask_UseGameplayAbility();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory) override;

protected:
	virtual EBTNodeResult::Type PerformUseGameplayAbilityTask(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory);
	virtual FString GetStaticDescription() const override;

protected:
	/** Ability tag used for activation. */
	UPROPERTY(EditAnywhere, meta=(Categories="AbilityActivation"), Category = "Obsidian")
	FGameplayTagContainer ActivateAbilityWithTag;

	/** Combat Target to set on EnemyInterface. Make sure to set this or the Combat Target will be nullptr! */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bSetCombatTargetOnEnemyInterface == true", EditConditionHides, DisplayName = "Combat Target"), Category = "Key")
	FBlackboardKeySelector CombatTarget_Selector;

	/** Should this task set the Combat Target on EnemyInterface. */
	UPROPERTY(EditAnywhere, Category = "Obsidian|CombatTarget")
	bool bSetCombatTargetOnEnemyInterface = false;

	UPROPERTY(EditAnywhere, Category = "Obsidian")
	bool bDebugEnabled = false;
};

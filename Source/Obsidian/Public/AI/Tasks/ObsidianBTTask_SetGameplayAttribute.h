// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "CoreMinimal.h"

#include "AbilitySystem/Abilities/OGameplayAbility_ProjectileSpell.h"

#include "ObsidianBTTask_SetGameplayAttribute.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FGameplayAttributeSet
{
	GENERATED_BODY()

	/** The Attribute we modify or the GE we modify modifies. */
	UPROPERTY(EditAnywhere, Category = "GameplayModifier")
	FGameplayAttribute GameplayAttribute;

	/** The numeric operation of this modifier: Override, Add, Multiply, etc  */
	UPROPERTY(EditAnywhere, Category = "GameplayModifier")
	TEnumAsByte<EGameplayModOp::Type> ModifierOp = EGameplayModOp::Additive;

	/** Magnitude of the attribute */
	UPROPERTY(EditAnywhere, Category = "GameplayModifier")
	FScalableFloat Magnitude;
};

/**
 * Task for giving Gameplay Attribute to the Behavior Tree Owner.
 */
UCLASS(meta = (PrioritizeCategories = "Duration Modifiers"))
class OBSIDIAN_API UObsidianBTTask_SetGameplayAttribute : public UBTTaskNode
{
	GENERATED_BODY()

public:
	//~ Begin UBTNode Interface
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface

	//~ Begin UBTTaskNode Interface
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory) override;
	//~ End UBTTaskNode Interface

protected:
	/** Policy for the duration of this effect */
	UPROPERTY(EditAnywhere, Category = "Duration")
	EGameplayEffectDurationType DurationPolicy;

	UPROPERTY(EditAnywhere, meta=(EditCondition="DurationPolicy == EGameplayEffectDurationType::HasDuration", EditConditionHides), Category = "Duration")
	float Duration = 0.0f;

	/** Array of modifiers that will affect the target of this effect */
	UPROPERTY(EditAnywhere, Category = "Modifiers")
	TArray<FGameplayAttributeSet> Modifiers;

private:
	UObsidianBTTask_SetGameplayAttribute();
};

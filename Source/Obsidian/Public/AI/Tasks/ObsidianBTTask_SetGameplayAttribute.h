// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "AbilitySystem/Abilities/OGameplayAbility_ProjectileSpell.h"

#include "BehaviorTree/BTTaskNode.h"
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
UCLASS()
class OBSIDIAN_API UObsidianBTTask_SetGameplayAttribute : public UBTTaskNode
{
	GENERATED_BODY()

	UObsidianBTTask_SetGameplayAttribute();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual FString GetStaticDescription() const override;
	
protected:
	/** Policy for the duration of this effect */
	UPROPERTY(EditAnywhere, Category = "Duration")
	EGameplayEffectDurationType DurationPolicy;

	UPROPERTY(EditAnywhere, meta=(EditCondition="DurationPolicy == EGameplayEffectDurationType::HasDuration", EditConditionHides), Category = "Duration")
	float Duration = 0.0f;
	
	/** Array of modifiers that will affect the target of this effect */
	UPROPERTY(EditAnywhere, Category = "Modifiers")
	TArray<FGameplayAttributeSet> Modifiers;
};

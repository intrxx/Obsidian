// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ObsidianGameplayAbility.generated.h"

class UObsidianAbilitySystemComponent;
/**
 * Defines how the Ability is meant to activate.
 */
UENUM(BlueprintType)
enum class EObsidianGameplayAbility_ActivationPolicy : uint8
{
	// Try to active the Ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the Ability while the input is active.
	WhileInputActive,

	// Try to active the ability when an avatar is assigned.
	OnSpawn
};

//TODO Think about Lyra like AbilityActivationGroup

/**
 * The base Gameplay Ability class used in this project.
 */
UCLASS()
class OBSIDIAN_API UObsidianGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend UObsidianAbilitySystemComponent;

public:
	UObsidianGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	EObsidianGameplayAbility_ActivationPolicy GetAbilityActivationPolicy() const {return ActivationPolicy;}

protected:
	//~UGameplayAbility interface
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	//~End of UGameplayAbility interface
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Ability Activation")
	EObsidianGameplayAbility_ActivationPolicy ActivationPolicy;
	
};

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
	// Try to activate the Ability when the input is triggered.
	EAP_OnInputTriggered UMETA(DisplayName = "On Input Triggered"),

	// Continually try to activate the Ability while the input is active.
	EAP_WhileInputActive UMETA(DisplayName = "While input Active"),

	// Try to activate the ability when an avatar is assigned.
	EAP_OnSpawn UMETA(DisplayName = "On Spawn")
};

//TODO Think about Lyra like AbilityActivationGroup

USTRUCT(BlueprintType)
struct FObsidianTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AbilityMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag MontageTag = FGameplayTag();
};

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

	EObsidianGameplayAbility_ActivationPolicy GetAbilityActivationPolicy() const
	{
		return ActivationPolicy;
	}

protected:
	//~UGameplayAbility interface
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	//~End of UGameplayAbility interface

	/** Randomly chooses the Anim Montage to play. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|AbilityAnims")
	UAnimMontage* GetRandomAnimMontageToPlay();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Ability Activation")
	EObsidianGameplayAbility_ActivationPolicy ActivationPolicy;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianTaggedMontage> AbilityMontages;
};

// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystem/Abilities/ObsidianGameplayAbility.h"

#include "ObsidianGameplayAbility_Death.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianGameplayAbility_Death : public UObsidianGameplayAbility
{
	GENERATED_BODY()

public:
	UObsidianGameplayAbility_Death();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, bool bInReplicateEndAbility, bool bInWasCancelled) override;

	/** Starts the death sequence. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Ability")
	void StartDeath();

	/** Finishes the death sequence. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Ability")
	void FinishDeath();

protected:
	/** If enabled, the ability will automatically call StartDeath. FinishDeath is always called when the ability ends if the death was started. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Death")
	bool bAutoStartDeath = true;
};

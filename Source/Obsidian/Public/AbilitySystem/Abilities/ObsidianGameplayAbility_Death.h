// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


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
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

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

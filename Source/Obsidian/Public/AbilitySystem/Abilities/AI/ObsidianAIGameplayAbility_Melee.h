// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ObsidianGameplayAbility_Melee.h"
#include "ObsidianAIGameplayAbility_Melee.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianAIGameplayAbility_Melee : public UObsidianGameplayAbility_Melee
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	bool bShouldStopMovement = false;
};

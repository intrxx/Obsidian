// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystem/Abilities/ObsidianGameplayAbility_Melee.h"
#include "AI/Services/ObsidianBTService_FindNearestPlayer.h"

#include "ObsidianAIGameplayAbility_Melee.generated.h"

class AAIController;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianAIGameplayAbility_Melee : public UObsidianGameplayAbility_Melee
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, bool bInReplicateEndAbility, bool bInWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Ability")
	AAIController* GetAIControllerFromActorInfo();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	bool bShouldStopMovement = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	bool bShouldRotateToTarget = false;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian")
	TObjectPtr<AActor> CombatTargetActor = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian")
	TObjectPtr<AAIController> OwningAIController = nullptr;
	
	EAIRequestPriority::Type RequestPriority = EAIRequestPriority::HardScript;
};

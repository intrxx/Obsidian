// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ObsidianGameplayAbility.h"
#include "ObsidianGameplayAbility_Aura.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianGameplayAbility_Aura : public UObsidianGameplayAbility
{
	GENERATED_BODY()

public:
	UObsidianGameplayAbility_Aura();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	/** Aura effect that will be applied to user */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|AbilitySetup")
	TSubclassOf<UGameplayEffect> AuraEffectClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|AbilitySetup")
	FGameplayTag EffectUIInfoTag;

private:
	FActiveGameplayEffectHandle AuraEffectHandle;

	TObjectPtr<const UObsidianAbilitySystemComponent> ObsidianASC;
};

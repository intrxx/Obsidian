// Copyright 2026 out of sCope team - intrxx

#include "AbilitySystem/Abilities/ObsidianGameplayAbility_Melee.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "Obsidian/ObsidianLogCategories.h"


FGameplayEffectSpecHandle UObsidianGameplayAbility_Melee::MakeMeleeDamageSpec(const UObject* InSourceObject)
{
	if (const UAbilitySystemComponent* OwningASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayEffectContextHandle ContextHandle = OwningASC->MakeEffectContext();
		ContextHandle.SetAbility(this);
		ContextHandle.AddSourceObject(InSourceObject);
	
		const FGameplayEffectSpecHandle SpecHandle = OwningASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);

		for(TTuple<FGameplayTag, FObsidianAbilityDamageRange>& Pair : BaseDamageTypeMap)
		{
			const float Damage = Pair.Value.RollForDamageNumberAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, Damage);
		}

		return SpecHandle;
	}
	
	UE_LOG(ObLogAbilitySystem, Error, TEXT("Could not extract Ability System Component from Owning Actor in [%hs]."), __FUNCTION__);
	return FGameplayEffectSpecHandle(nullptr);
}

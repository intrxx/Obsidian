// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/Abilities/ObsidianGameplayAbility_Melee.h"

#include <AbilitySystemComponent.h>

#include "AbilitySystemBlueprintLibrary.h"
#include "Obsidian/ObsidianGameModule.h"

FGameplayEffectSpecHandle UObsidianGameplayAbility_Melee::MakeMeleeDamageSpec(const UObject* SourceObject)
{
	if (const UAbilitySystemComponent* OwningASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayEffectContextHandle ContextHandle = OwningASC->MakeEffectContext();
		ContextHandle.SetAbility(this);
		ContextHandle.AddSourceObject(SourceObject);
	
		const FGameplayEffectSpecHandle SpecHandle = OwningASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);

		for(TTuple<FGameplayTag, FObsidianAbilityDamageRange>& Pair : BaseDamageTypeMap)
		{
			const float Damage = Pair.Value.RollForDamageNumberAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, Damage);
		}

		return SpecHandle;
	}
	
	UE_LOG(LogObsidian, Error, TEXT("Could not extract Ability System Component from Owning Actor in [%hs]."), __FUNCTION__);
	return FGameplayEffectSpecHandle(nullptr);
}

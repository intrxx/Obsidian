// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Attributes/ObsidianEnemyAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "CharacterComponents/ObsidianBossComponent.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "Net/UnrealNetwork.h"
#include "Obsidian/ObsidianGameplayTags.h"

UObsidianEnemyAttributeSet::UObsidianEnemyAttributeSet()
{
}

void UObsidianEnemyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, HitReactThreshold, COND_None, REPNOTIFY_Always);
}

void UObsidianEnemyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	//FObsidianEffectProperties EffectProps;
	//SetEffectProperties(Data, /** OUT */ EffectProps);

	if(Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		const float CurrentHealth = GetHealth();
		
		if(EffectProps.bCanHitReact && CurrentHealth > 0.0f)
		{
			//TODO As it stands now only damage that damages health will cause a hit react - decide if its okay, I like it for now
			// Check if we should hit react, small hits shouldn't cause hit reacting
			const float CombinedHealthPool = GetMaxHealth() + GetMaxEnergyShield();
			if((LocalIncomingDamage / CombinedHealthPool) * 100.f > GetHitReactThreshold())
			{
				UAbilitySystemComponent* TargetASC = EffectProps.TargetASC;
				TargetASC->CancelAllAbilities();
				
				FGameplayTagContainer ActivateTag;
				ActivateTag.AddTag(ObsidianGameplayTags::AbilityActivation_HitReact);
				TargetASC->TryActivateAbilitiesByTag(ActivateTag);
			}	
		}

		if(EffectProps.bIsBoss && CurrentHealth > 0.0f)
		{
			if(const UObsidianBossComponent* BossComponent = UObsidianBossComponent::FindBossComponent(EffectProps.TargetAvatarActor))
			{
				if(CurrentHealth / GetMaxHealth() <= 0.50f)
				{
					BossComponent->OnBossThresholdReached_50Delegate.Broadcast();
				}
			}
		}
	}

	ResetMetaAttributes();
	EffectProps.Reset();
}

void UObsidianEnemyAttributeSet::OnRep_HitReactThreshold(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UObsidianEnemyAttributeSet, HitReactThreshold, OldValue);
}

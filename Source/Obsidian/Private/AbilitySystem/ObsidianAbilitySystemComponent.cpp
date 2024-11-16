// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "GameplayCueManager.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/Data/OAbilityTagRelationshipMapping.h"
#include "AbilitySystem/Abilities/ObsidianGameplayAbility.h"
#include "Obsidian/ObsidianGameplayTags.h"


UObsidianAbilitySystemComponent::UObsidianAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UObsidianAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if(InputTag.IsValid())
	{
		for(const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
		{
			if(AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UObsidianAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
		{
			if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UObsidianAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);
	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UObsidianAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);
	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UObsidianAbilitySystemComponent::ClientOnEffectApplied_Implementation(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec,
	FActiveGameplayEffectHandle EffectHandle)
{
	FGameplayTagContainer AssetTags;
	EffectSpec.GetAllAssetTags(AssetTags);
	
	if(AssetTags.HasTagExact(ObsidianGameplayTags::UI_DataSpecifierTag))
	{
		FObsidianEffectUIData EffectUIData;
		
		EffectUIData.AssetTags = AssetTags;

		const EGameplayEffectDurationType GameplayEffectType = EffectSpec.Def->DurationPolicy;
		EffectUIData.EffectDurationPolicy = GameplayEffectType;

		if(GameplayEffectType == EGameplayEffectDurationType::HasDuration)
		{
			EffectUIData.EffectDuration = EffectSpec.GetDuration();
			EffectUIData.EffectMagnitude = CalculateFullEffectMagnitude(EffectSpec);
		}
	
		if(EffectSpec.Def->StackingType != EGameplayEffectStackingType::None)
		{
			EffectUIData.bStackingEffect = true;
		
			FObsidianEffectUIStackingData StackingData;
		
			StackingData.EffectStackCount = EffectSpec.GetStackCount();
			StackingData.EffectExpirationDurationPolicy = EffectSpec.Def->GetStackExpirationPolicy();
			StackingData.EffectStackingDurationPolicy = EffectSpec.Def->StackDurationRefreshPolicy;
			EffectUIData.StackingData = StackingData;
		}
	
		OnEffectAppliedAssetTags.Broadcast(EffectUIData);
	}
}

float UObsidianAbilitySystemComponent::CalculateFullEffectMagnitude(const FGameplayEffectSpec& EffectSpec)
{
	//@Hack It fixes a crash when applying cooldown effects as cds have no modifiers
	if(EffectSpec.Def->Modifiers.Num() == 0)
	{
		return 0.f;
	}
	
	const float Magnitude = EffectSpec.GetModifierMagnitude(0, false);
	
	const float Duration = EffectSpec.GetDuration();
	const float Period = EffectSpec.GetPeriod();
	
	float FullMagnitude = Duration / Period * Magnitude;
	
	if(EffectSpec.Def->bExecutePeriodicEffectOnApplication)
	{
		FullMagnitude += Magnitude;
		//UE_LOG(LogObsidian, Warning, TEXT("%f / %f * %f + %f = %f"),Duration, Period, Magnitude, Magnitude, FullMagnitude);
	}
	//UE_LOG(LogObsidian, Warning, TEXT("%f / %f * %f = %f"),Duration, Period, Magnitude, FullMagnitude);
	
	return FullMagnitude;
}

void UObsidianAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bPauseGame)
{
	//TODO Check for blocking tag here and clear input if this ASC has it
	
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// Process all abilities that activate when the input is held.
	for(const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if(const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if(AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UObsidianGameplayAbility* AbilityCDO = CastChecked<UObsidianGameplayAbility>(AbilitySpec->Ability);

				if(AbilityCDO->GetAbilityActivationPolicy() == EObsidianGameplayAbility_ActivationPolicy::EAP_WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	// Process all abilities that had their input pressed this frame.
	for(const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if(FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if(AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if(AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UObsidianGameplayAbility* AbilityCDO = CastChecked<UObsidianGameplayAbility>(AbilitySpec->Ability);

					if (AbilityCDO->GetAbilityActivationPolicy() == EObsidianGameplayAbility_ActivationPolicy::EAP_OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	//FROM LYRA
	// Try to activate all the abilities that are from presses and holds.
	// We do it all at once so that held inputs don't activate the ability
	// and then also send an input event to the ability because of the press.
	for(const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	// Process all abilities that had their input released this frame.
	for(const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if(FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if(AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if(AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}
	
	// Clear the cached ability handles.
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UObsidianAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UObsidianAbilitySystemComponent::SetTagRelationshipMapping(UOAbilityTagRelationshipMapping* MappingToSet)
{
	TagRelationshipMapping = MappingToSet;
}

void UObsidianAbilitySystemComponent::AbilityActorInfoSet()
{
	BindToOnEffectAppliedDelegate();
}

void UObsidianAbilitySystemComponent::BindToOnEffectAppliedDelegate()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::ClientOnEffectApplied);
}

void UObsidianAbilitySystemComponent::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags,
                                                                             FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
{
	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, &OutActivationRequired, &OutActivationBlocked);
	}
}

void UObsidianAbilitySystemComponent::ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::Executed, GameplayCueParameters);
}

void UObsidianAbilitySystemComponent::AddGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::OnActive, GameplayCueParameters);
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::WhileActive, GameplayCueParameters);
}

void UObsidianAbilitySystemComponent::RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::Removed, GameplayCueParameters);
}


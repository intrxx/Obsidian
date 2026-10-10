// Copyright 2026 out of sCope team - intrxx

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"

#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"

#include "AbilitySystem/Abilities/ObsidianGameplayAbility.h"
#include "AbilitySystem/Data/OAbilityTagRelationshipMapping.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "Obsidian/ObsidianLogCategories.h"


UObsidianAbilitySystemComponent::UObsidianAbilitySystemComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UObsidianAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InInputTag)
{
	if(InInputTag.IsValid())
	{
		for(const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
		{
			if(AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UObsidianAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InInputTag)
{
	if (InInputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
		{
			if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UObsidianAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& InSpec)
{
	Super::AbilitySpecInputPressed(InSpec);
	if (InSpec.IsActive())
	{
PRAGMA_DISABLE_DEPRECATION_WARNINGS
		const UGameplayAbility* Instance = InSpec.GetPrimaryInstance();
		const FPredictionKey InstancedPredictionKey = Instance ? Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : InSpec.ActivationInfo.GetActivationPredictionKey();
PRAGMA_DISABLE_DEPRECATION_WARNINGS
		
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, InSpec.Handle, InstancedPredictionKey);
	}
}

void UObsidianAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& InSpec)
{
	Super::AbilitySpecInputReleased(InSpec);
	if (InSpec.IsActive())
	{
PRAGMA_DISABLE_DEPRECATION_WARNINGS
		const UGameplayAbility* Instance = InSpec.GetPrimaryInstance();
		const FPredictionKey InstancedPredictionKey = Instance ? Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : InSpec.ActivationInfo.GetActivationPredictionKey();
PRAGMA_DISABLE_DEPRECATION_WARNINGS

		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, InSpec.Handle, InstancedPredictionKey);
	}
}

void UObsidianAbilitySystemComponent::ClientOnEffectApplied_Implementation(UAbilitySystemComponent* InASC, const FGameplayEffectSpec& InEffectSpec,
	FActiveGameplayEffectHandle InEffectHandle)
{
	FGameplayTagContainer AssetTags;
	InEffectSpec.GetAllAssetTags(AssetTags);
	
	if(AssetTags.HasTagExact(ObsidianGameplayTags::UI::DataSpecifierTag))
	{
		FObsidianEffectUIData EffectUIData;
		
		EffectUIData.AssetTags = AssetTags;

		const EGameplayEffectDurationType GameplayEffectType = InEffectSpec.Def->DurationPolicy;
		EffectUIData.EffectDurationPolicy = GameplayEffectType;

		if(GameplayEffectType == EGameplayEffectDurationType::HasDuration)
		{
			EffectUIData.EffectDuration = InEffectSpec.GetDuration();
			EffectUIData.EffectMagnitude = CalculateFullEffectMagnitude(InEffectSpec);
		}
	
		if(InEffectSpec.Def->StackingType != EGameplayEffectStackingType::None)
		{
			EffectUIData.bStackingEffect = true;
		
			FObsidianEffectUIStackingData StackingData;
		
			StackingData.EffectStackCount = InEffectSpec.GetStackCount();
			StackingData.EffectExpirationDurationPolicy = InEffectSpec.Def->GetStackExpirationPolicy();
			StackingData.EffectStackingDurationPolicy = InEffectSpec.Def->StackDurationRefreshPolicy;
			EffectUIData.StackingData = StackingData;
		}
	
		OnEffectAppliedAssetTags.Broadcast(EffectUIData);
	}
}

float UObsidianAbilitySystemComponent::CalculateFullEffectMagnitude(const FGameplayEffectSpec& InEffectSpec)
{
	//@Hack It fixes a crash when applying cooldown effects as cds have no modifiers
	if(InEffectSpec.Def->Modifiers.Num() == 0)
	{
		return 0.f;
	}
	
	const float Magnitude = InEffectSpec.GetModifierMagnitude(0, false);
	
	const float Duration = InEffectSpec.GetDuration();
	const float Period = InEffectSpec.GetPeriod();
	
	float FullMagnitude = Duration / Period * Magnitude;
	
	if(InEffectSpec.Def->bExecutePeriodicEffectOnApplication)
	{
		FullMagnitude += Magnitude;
	}
	
	return FullMagnitude;
}

void UObsidianAbilitySystemComponent::ProcessAbilityInput(float InDeltaTime, bool bInPauseGame)
{
	//TODO(intrxx) Check for blocking tag here and clear input if this ASC has it
	
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

void UObsidianAbilitySystemComponent::SetTagRelationshipMapping(UOAbilityTagRelationshipMapping* InMappingToSet)
{
	TagRelationshipMapping = InMappingToSet;
}

void UObsidianAbilitySystemComponent::AbilityActorInfoSet()
{
	BindToOnEffectAppliedDelegate();
}

void UObsidianAbilitySystemComponent::BindToOnEffectAppliedDelegate()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::ClientOnEffectApplied);
}

void UObsidianAbilitySystemComponent::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& InAbilityTags,
                                                                             FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
{
	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(InAbilityTags, &OutActivationRequired, &OutActivationBlocked);
	}
}

void UObsidianAbilitySystemComponent::ExecuteGameplayCueLocal(const FGameplayTag InGameplayCueTag, const FGameplayCueParameters& InGameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), InGameplayCueTag, EGameplayCueEvent::Type::Executed, InGameplayCueParameters);
}

void UObsidianAbilitySystemComponent::AddGameplayCueLocal(const FGameplayTag InGameplayCueTag, const FGameplayCueParameters& InGameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), InGameplayCueTag, EGameplayCueEvent::Type::OnActive, InGameplayCueParameters);
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), InGameplayCueTag, EGameplayCueEvent::Type::WhileActive, InGameplayCueParameters);
}

void UObsidianAbilitySystemComponent::RemoveGameplayCueLocal(const FGameplayTag InGameplayCueTag, const FGameplayCueParameters& InGameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), InGameplayCueTag, EGameplayCueEvent::Type::Removed, InGameplayCueParameters);
}


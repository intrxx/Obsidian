// Copyright 2026 out of sCope team - intrxx

#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"

#include <GameFramework/Character.h>

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "CharacterComponents/Movement/ObsidianHeroMovementComponent.h"
#include "Obsidian/ObsidianGameModule.h"

UObsidianHeroAttributesComponent::UObsidianHeroAttributesComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HeroAttributeSet = nullptr;
}

void UObsidianHeroAttributesComponent::InitializeWithAbilitySystem(UObsidianAbilitySystemComponent* InASC, ACharacter* Owner)
{
	check(Owner);
	
	Super::InitializeWithAbilitySystem(InASC, Owner);

	HeroAttributeSet = AbilitySystemComponent->GetSet<UObsidianHeroAttributeSet>();
	if (!HeroAttributeSet)
	{
		UE_LOG(LogObsidian, Error, TEXT("[%hs] Cannot initialize Hero Attributes Component for owner [%s] with NULL Hero Set set on the Ability System."), __FUNCTION__, *GetNameSafe(Owner));
		return;
	}

	ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetManaAttribute()).AddUObject(this, &ThisClass::ManaChanged);
	MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxManaAttribute()).AddUObject(this, &ThisClass::MaxManaChanged);

	HeroAttributeSet->OnOutOfStamina.AddUObject(this, &ThisClass::HandleOutOfStamina);
	
	// Set the Mana value to the MaxMana
	AbilitySystemComponent->SetNumericAttributeBase(GetManaAttribute(), GetMaxMana());

	bIsLocallyController = Owner->IsLocallyControlled();
	WeakOwner = Owner;
}

void UObsidianHeroAttributesComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();
	
	ManaChangedDelegateHandle.Reset();
	MaxManaChangedDelegateHandle.Reset();
	
	HeroAttributeSet = nullptr;
	
	Super::UninitializeFromAbilitySystem();
}

UGameplayEffect* UObsidianHeroAttributesComponent::GetLevelUpEffect() const
{
	if(LevelUpEffectClass)
	{
		return LevelUpEffectClass->GetDefaultObject<UGameplayEffect>();
	}
	return nullptr;
}

FObsidianSpecialResourceVisuals UObsidianHeroAttributesComponent::GetSpecialResourceVisuals() const
{
	return SpecialResourceVisuals;
}


void UObsidianHeroAttributesComponent::ClearGameplayTags()
{
	Super::ClearGameplayTags();
}

void UObsidianHeroAttributesComponent::HealthChanged(const FOnAttributeChangeData& Data)
{
	// Broadcast for simulated hero health bar
	if (!bIsLocallyController)
	{
		OnHeroHealthChangedDelegate.Broadcast(Data.NewValue);
	}
}

void UObsidianHeroAttributesComponent::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	// Broadcast for simulated hero health bar
	if(!bIsLocallyController)
	{
		OnHeroMaxHealthChangedDelegate.Broadcast(Data.NewValue);
	}
}

void UObsidianHeroAttributesComponent::EnergyShieldChanged(const FOnAttributeChangeData& Data)
{
	// Broadcast for simulated hero health bar
	if(!bIsLocallyController)
	{
		OnHeroEnergyShieldChangedDelegate.Broadcast(Data.NewValue);
	}
}

void UObsidianHeroAttributesComponent::MaxEnergyShieldChanged(const FOnAttributeChangeData& Data)
{
	// Broadcast for simulated hero health bar
	if(!bIsLocallyController)
	{
		OnHeroMaxEnergyShieldChangedDelegate.Broadcast(Data.NewValue);
	}
}

void UObsidianHeroAttributesComponent::ManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogObsidian, Warning, TEXT("Hero - Implement Mana Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::MaxManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogObsidian, Warning, TEXT("Hero - Implement Max Mana Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::HandleOutOfStamina(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	if (WeakOwner.IsValid())
	{
		UObsidianHeroMovementComponent* MoveComp = Cast<UObsidianHeroMovementComponent>(
			WeakOwner.Get()->GetCharacterMovement());
		if (MoveComp)
		{
			MoveComp->HandleOutOfStamina();
		}
	}
}

float UObsidianHeroAttributesComponent::GetMana() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetMana() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetManaAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetManaAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetMaxMana() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetMaxMana() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetMaxManaAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetMaxManaAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetStamina() const
{
return (HeroAttributeSet ? HeroAttributeSet->GetStamina() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetStaminaAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetStaminaAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetMaxStamina() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetMaxStamina() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetMaxStaminaAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetMaxStaminaAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetSprintSpeed() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetSprintSpeed() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetSprintSpeedAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetSprintSpeedAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetSpecialResource() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetSpecialResource() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetSpecialResourceAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetSpecialResourceAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetMaxSpecialResource() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetMaxSpecialResource() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetMaxSpecialResourceAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetMaxSpecialResourceAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetManaRegeneration() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetManaRegeneration() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetManaRegenerationAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetManaRegenerationAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetStaminaRegeneration() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetStaminaRegeneration() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetStaminaRegenerationAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetStaminaRegenerationAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetExperience() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetExperience() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetExperienceAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetExperienceAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetMaxExperience() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetMaxExperience() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetMaxExperienceAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetMaxExperienceAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetPassiveSkillPoints() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetPassiveSkillPoints() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetPassiveSkillPointsAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetPassiveSkillPointsAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetAscensionPoints() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetAscensionPoints() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetAscensionPointsAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetAscensionPointsAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetStrength() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetStrength() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetStrengthAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetStrengthAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetIntelligence() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetIntelligence() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetIntelligenceAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetIntelligenceAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetDexterity() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetDexterity() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetDexterityAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetDexterityAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetFaith() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetFaith() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetFaithAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetFaithAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetHitBlockChance() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetHitBlockChance() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetHitBlockChanceAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetHitBlockChanceAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetMaxHitBlockChance() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetMaxHitBlockChance() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetMaxHitBlockChanceAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetMaxHitBlockChanceAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetSpellBlockChance() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetSpellBlockChance() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetSpellBlockChanceAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetSpellBlockChanceAttribute() : nullptr);
}

float UObsidianHeroAttributesComponent::GetMaxSpellBlockChance() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetMaxSpellBlockChance() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetMaxSpellBlockChanceAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetMaxSpellBlockChanceAttribute() : nullptr);
}






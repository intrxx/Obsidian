// Copyright 2024 out of sCope team - Michał Ogiński


#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "GameFramework/Character.h"
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
		UE_LOG(LogObsidian, Error, TEXT("ObsidianAttributesComponent: Cannot initialize Hero Attributes Component for owner [%s] with NULL Hero Set set on the Ability System."), *GetNameSafe(Owner));
		return;
	}

	ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetManaAttribute()).AddUObject(this, &ThisClass::ManaChanged);
	MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxManaAttribute()).AddUObject(this, &ThisClass::MaxManaChanged);
	
	// Set the Mana value to the MaxMana
	AbilitySystemComponent->SetNumericAttributeBase(GetManaAttribute(), GetMaxMana());

	bIsLocallyController = Owner->IsLocallyControlled();
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






// Copyright 2024 out of sCope team - Michał Ogiński


#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"

UObsidianHeroAttributesComponent::UObsidianHeroAttributesComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HeroAttributeSet = nullptr;
}

void UObsidianHeroAttributesComponent::InitializeWithAbilitySystem(UObsidianAbilitySystemComponent* InASC, AActor* Owner)
{
	AActor* CurrentOwner = GetOwner();
	check(CurrentOwner);
	
	Super::InitializeWithAbilitySystem(InASC, CurrentOwner);

	HeroAttributeSet = AbilitySystemComponent->GetSet<UObsidianHeroAttributeSet>();
	if (!HeroAttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("ObsidianAttributesComponent: Cannot initialize Hero Attributes Component for owner [%s] with NULL Hero Set set on the Ability System."), *GetNameSafe(Owner));
		return;
	}

	ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetManaAttribute()).AddUObject(this, &ThisClass::ManaChanged);
	MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxManaAttribute()).AddUObject(this, &ThisClass::MaxManaChanged);
	ManaRegenerationChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetManaRegenerationAttribute()).AddUObject(this, &ThisClass::ManaRegenerationChanged);
	StrengthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetStrengthAttribute()).AddUObject(this, &ThisClass::StrengthChanged);
	IntelligenceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetIntelligenceAttribute()).AddUObject(this, &ThisClass::IntelligenceChanged);
	DexterityChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetDexterityAttribute()).AddUObject(this, &ThisClass::DexterityChanged);
	FaithChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetFaithAttribute()).AddUObject(this, &ThisClass::FaithChanged);
	
	// Set the Mana value to the MaxMana
	AbilitySystemComponent->SetNumericAttributeBase(GetManaAttribute(), GetMaxMana());
}

void UObsidianHeroAttributesComponent::UninitializeFromAbilitySystem()
{
	ManaChangedDelegateHandle.Reset();
	MaxManaChangedDelegateHandle.Reset();
	StrengthChangedDelegateHandle.Reset();
	IntelligenceChangedDelegateHandle.Reset();
	DexterityChangedDelegateHandle.Reset();
	FaithChangedDelegateHandle.Reset();
	
	HeroAttributeSet = nullptr;
	
	Super::UninitializeFromAbilitySystem();
}

void UObsidianHeroAttributesComponent::HealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Health Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Max Health Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::EnergyShieldChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Energy Shield Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::MaxEnergyShieldChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Max Energy Shield Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::ManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Mana Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::MaxManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Max Mana Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::HealthRegenerationChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Health Regeneration Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::EnergyShieldRegenerationChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Energy Shield Regeneration Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::ManaRegenerationChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Mana Regeneration Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::StrengthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Strength Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::IntelligenceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Intelligence Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::DexterityChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Dexterity Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::FaithChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Faith Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::ArmorChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Armor Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::EvasionChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Evasion Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::FireResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Fire Res Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::MaxFireResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Max Fire Res Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::ColdResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Cold Res Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::MaxColdResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Max Cold Res Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::LightningResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Lightning Res Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::MaxLightningResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Max Lightning Res Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::ChaosResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Cold Res Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::MaxChaosResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Max Chaos Res Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::CriticalStrikeChanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Critical Strike Chance Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::CriticalStrikeMultiplierChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Crit Strike Multiplier Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::AttackSpeedChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Attack Speed Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::CastSpeedChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Cast Speed Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
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

float UObsidianHeroAttributesComponent::GetManaRegeneration() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetManaRegeneration() : 0.0f);
}

FGameplayAttribute UObsidianHeroAttributesComponent::GetManaRegenerationAttribute() const
{
	return (HeroAttributeSet ? HeroAttributeSet->GetManaRegenerationAttribute() : nullptr);
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





// Copyright 2024 Michał Ogiński


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
	StrengthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetStrengthAttribute()).AddUObject(this, &ThisClass::StrengthChanged);
	IntelligenceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetIntelligenceAttribute()).AddUObject(this, &ThisClass::IntelligenceChanged);
	DexterityChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetDexterityAttribute()).AddUObject(this, &ThisClass::DexterityChanged);
	ArmorChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetArmorAttribute()).AddUObject(this, &ThisClass::ArmorChanged);
	EvasionChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetEvasionAttribute()).AddUObject(this, &ThisClass::EvasionChanged);
	FireResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetFireResistanceAttribute()).AddUObject(this, &ThisClass::FireResistanceChanged);
	MaxFireResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxFireResistanceAttribute()).AddUObject(this, &ThisClass::MaxFireResistanceChanged);
	ColdResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetColdResistanceAttribute()).AddUObject(this, &ThisClass::ColdResistanceChanged);
	MaxColdResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxColdResistanceAttribute()).AddUObject(this, &ThisClass::MaxColdResistanceChanged);
	LightningResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetLightningResistanceAttribute()).AddUObject(this, &ThisClass::LightningResistanceChanged);
	MaxLightningResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxLightningResistanceAttribute()).AddUObject(this, &ThisClass::MaxLightningResistanceChanged);
	ChaosResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetChaosResistanceAttribute()).AddUObject(this, &ThisClass::ChaosResistanceChanged);
	MaxChaosResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxChaosResistanceAttribute()).AddUObject(this, &ThisClass::MaxChaosResistanceChanged);
	CriticalStrikeChanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetCriticalStrikeChanceAttribute()).AddUObject(this, &ThisClass::CriticalStrikeChanceChanged);
	CriticalStrikeMultiplierChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetCriticalStrikeMultiplierAttribute()).AddUObject(this, &ThisClass::CriticalStrikeMultiplierChanged);
	AttackSpeedChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAttackSpeedAttribute()).AddUObject(this, &ThisClass::AttackSpeedChanged);
	CastSpeedChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetCastSpeedAttribute()).AddUObject(this, &ThisClass::CastSpeedChanged);
	
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
	ArmorChangedDelegateHandle.Reset();
	EvasionChangedDelegateHandle.Reset();
	FireResistanceChangedDelegateHandle.Reset();
	MaxFireResistanceChangedDelegateHandle.Reset();
	ColdResistanceChangedDelegateHandle.Reset();
	MaxColdResistanceChangedDelegateHandle.Reset();
	LightningResistanceChangedDelegateHandle.Reset();
	MaxLightningResistanceChangedDelegateHandle.Reset();
	ChaosResistanceChangedDelegateHandle.Reset();
	MaxChaosResistanceChangedDelegateHandle.Reset();
	CriticalStrikeChanceChangedDelegateHandle.Reset();
	CriticalStrikeMultiplierChangedDelegateHandle.Reset();
	AttackSpeedChangedDelegateHandle.Reset();
	CastSpeedChangedDelegateHandle.Reset();
	
	HeroAttributeSet = nullptr;
	
	Super::UninitializeFromAbilitySystem();
}

void UObsidianHeroAttributesComponent::SetMainWidgetController(UMainOverlayWidgetController* InWidgetController)
{
	MainOverlayWidgetController = InWidgetController;

	// Initial values
	MainOverlayWidgetController->OnHealthChangedDelegate.Broadcast(GetHealth());
	MainOverlayWidgetController->OnMaxHealthChangedDelegate.Broadcast(GetMaxHealth());
	MainOverlayWidgetController->OnManaChangedDelegate.Broadcast(GetMana());
	MainOverlayWidgetController->OnMaxManaChangedDelegate.Broadcast(GetMaxMana());
}

void UObsidianHeroAttributesComponent::HealthChanged(const FOnAttributeChangeData& Data)
{
	if(MainOverlayWidgetController)
	{
		MainOverlayWidgetController->OnHealthChangedDelegate.Broadcast(Data.NewValue);
	}
}

void UObsidianHeroAttributesComponent::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	if(MainOverlayWidgetController)
	{
		MainOverlayWidgetController->OnMaxHealthChangedDelegate.Broadcast(Data.NewValue);
	}
}

void UObsidianHeroAttributesComponent::EnergyShieldChanged(const FOnAttributeChangeData& Data)
{
	
}

void UObsidianHeroAttributesComponent::MaxEnergyShieldChanged(const FOnAttributeChangeData& Data)
{
	
}

void UObsidianHeroAttributesComponent::ManaChanged(const FOnAttributeChangeData& Data)
{
	if(MainOverlayWidgetController)
	{
		MainOverlayWidgetController->OnManaChangedDelegate.Broadcast(Data.NewValue);
	}
}

void UObsidianHeroAttributesComponent::MaxManaChanged(const FOnAttributeChangeData& Data)
{
	if(MainOverlayWidgetController)
	{
		MainOverlayWidgetController->OnMaxManaChangedDelegate.Broadcast(Data.NewValue);
	}
}

void UObsidianHeroAttributesComponent::StrengthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Strength Changed!"));
}

void UObsidianHeroAttributesComponent::IntelligenceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Intelligence Changed!"));
}

void UObsidianHeroAttributesComponent::DexterityChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Dexterity Changed!"));
}

void UObsidianHeroAttributesComponent::ArmorChanged(const FOnAttributeChangeData& Data)
{
}

void UObsidianHeroAttributesComponent::EvasionChanged(const FOnAttributeChangeData& Data)
{
}

void UObsidianHeroAttributesComponent::FireResistanceChanged(const FOnAttributeChangeData& Data)
{
}

void UObsidianHeroAttributesComponent::MaxFireResistanceChanged(const FOnAttributeChangeData& Data)
{
}

void UObsidianHeroAttributesComponent::ColdResistanceChanged(const FOnAttributeChangeData& Data)
{
}

void UObsidianHeroAttributesComponent::MaxColdResistanceChanged(const FOnAttributeChangeData& Data)
{
}

void UObsidianHeroAttributesComponent::LightningResistanceChanged(const FOnAttributeChangeData& Data)
{
}

void UObsidianHeroAttributesComponent::MaxLightningResistanceChanged(const FOnAttributeChangeData& Data)
{
}

void UObsidianHeroAttributesComponent::ChaosResistanceChanged(const FOnAttributeChangeData& Data)
{
}

void UObsidianHeroAttributesComponent::MaxChaosResistanceChanged(const FOnAttributeChangeData& Data)
{
}

void UObsidianHeroAttributesComponent::CriticalStrikeChanceChanged(const FOnAttributeChangeData& Data)
{
}

void UObsidianHeroAttributesComponent::CriticalStrikeMultiplierChanged(const FOnAttributeChangeData& Data)
{
}

void UObsidianHeroAttributesComponent::AttackSpeedChanged(const FOnAttributeChangeData& Data)
{
}

void UObsidianHeroAttributesComponent::CastSpeedChanged(const FOnAttributeChangeData& Data)
{
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




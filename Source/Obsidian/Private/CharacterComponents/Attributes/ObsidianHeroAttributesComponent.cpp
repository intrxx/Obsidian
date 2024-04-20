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

void UObsidianHeroAttributesComponent::SetMainWidgetController(UMainOverlayWidgetController* InWidgetController)
{
	MainOverlayWidgetController = InWidgetController;

	/**
	 * Initial values
	 */
	 
	MainOverlayWidgetController->OnHealthChangedDelegate.Broadcast(GetHealth());
	MainOverlayWidgetController->OnMaxHealthChangedDelegate.Broadcast(GetMaxHealth());
	MainOverlayWidgetController->OnManaChangedDelegate.Broadcast(GetMana());
	MainOverlayWidgetController->OnMaxManaChangedDelegate.Broadcast(GetMaxMana());
	MainOverlayWidgetController->OnEnergyShieldChangedDelegate.Broadcast(GetEnergyShield());
	MainOverlayWidgetController->OnMaxEnergyShieldChangedDelegate.Broadcast(GetMaxEnergyShield());
	
	/**
	 *
	 */
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
	if(MainOverlayWidgetController)
	{
		MainOverlayWidgetController->OnEnergyShieldChangedDelegate.Broadcast(Data.NewValue);
	}
}

void UObsidianHeroAttributesComponent::MaxEnergyShieldChanged(const FOnAttributeChangeData& Data)
{
	if(MainOverlayWidgetController)
	{
		MainOverlayWidgetController->OnMaxEnergyShieldChangedDelegate.Broadcast(Data.NewValue);
	}
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

void UObsidianHeroAttributesComponent::HealthRegenerationChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Health Regeneration Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::EnergyShieldRegenerationChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Energy Shield Regeneration Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::ManaRegenerationChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Mana Regeneration Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::StrengthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Strength Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::IntelligenceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Intelligence Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::DexterityChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Dexterity Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::FaithChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Faith Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::ArmorChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Armor Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::EvasionChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Evasion Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::FireResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Fire Res Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::MaxFireResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Max Fire Res Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::ColdResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Cold Res Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::MaxColdResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Max Cold Res Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::LightningResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Lightning Res Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::MaxLightningResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Max Lightning Res Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::ChaosResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Cold Res Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::MaxChaosResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Max Chaos Res Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::CriticalStrikeChanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Critical Strike Chance Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::CriticalStrikeMultiplierChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Crit Strike Multiplier Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::AttackSpeedChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Attack Speed Changed! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianHeroAttributesComponent::CastSpeedChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hero - Implement Cast Speed Changed! - For %s"), *GetNameSafe(GetOwner()));
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





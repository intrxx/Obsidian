// Copyright 2024 out of sCope team - Michał Ogiński


#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "Obsidian/Obsidian.h"
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
		UE_LOG(LogObsidian, Error, TEXT("ObsidianAttributesComponent: Cannot initialize Hero Attributes Component for owner [%s] with NULL Hero Set set on the Ability System."), *GetNameSafe(Owner));
		return;
	}

	ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetManaAttribute()).AddUObject(this, &ThisClass::ManaChanged);
	MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxManaAttribute()).AddUObject(this, &ThisClass::MaxManaChanged);
	
	// Set the Mana value to the MaxMana
	AbilitySystemComponent->SetNumericAttributeBase(GetManaAttribute(), GetMaxMana());
}

void UObsidianHeroAttributesComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();
	
	ManaChangedDelegateHandle.Reset();
	MaxManaChangedDelegateHandle.Reset();
	
	HeroAttributeSet = nullptr;
	
	Super::UninitializeFromAbilitySystem();
}


void UObsidianHeroAttributesComponent::ClearGameplayTags()
{
	Super::ClearGameplayTags();
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






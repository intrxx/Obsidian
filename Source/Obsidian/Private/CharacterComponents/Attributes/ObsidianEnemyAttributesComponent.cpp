// Copyright 2026 out of sCope team - intrxx

#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"

#include "GameFramework/Character.h"

#include "AbilitySystem/Attributes/ObsidianEnemyAttributeSet.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Obsidian/ObsidianLogCategories.h"


UObsidianEnemyAttributesComponent::UObsidianEnemyAttributesComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	EnemyAttributeSet = nullptr;
}

void UObsidianEnemyAttributesComponent::InitializeWithAbilitySystem(UObsidianAbilitySystemComponent* InASC, ACharacter* InOwner)
{
	check(InOwner);
	
	Super::InitializeWithAbilitySystem(InASC, InOwner);

	EnemyAttributeSet = AbilitySystemComponent->GetSet<UObsidianEnemyAttributeSet>();
	if (!EnemyAttributeSet)
	{
		UE_LOG(ObLogAttributes, Error, TEXT("ObsidianEnemyAttributesComponent: Cannot initialize Attributes Component for owner [%s] with NULL Enemy Set set on the Ability System."), *GetNameSafe(InOwner));
		return;
	}

	StaggerMeterDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetStaggerMeterAttribute()).AddUObject(this, &ThisClass::StaggerMeterChanged);
	MaxStaggerMeterDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxStaggerMeterAttribute()).AddUObject(this, &ThisClass::MaxStaggerMeterChanged);
	
	BroadcastInitialValues();
}

void UObsidianEnemyAttributesComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();

	StaggerMeterDelegateHandle.Reset();
	MaxStaggerMeterDelegateHandle.Reset();
	
	EnemyAttributeSet = nullptr;
	
	Super::UninitializeFromAbilitySystem();
}

void UObsidianEnemyAttributesComponent::BroadcastInitialValues() const
{
	HealthChangedDelegate.Broadcast(GetHealth());
	MaxHealthChangedDelegate.Broadcast(GetMaxHealth());
	EnergyShieldChangedDelegate.Broadcast(GetEnergyShield());
	MaxEnergyShieldChangedDelegate.Broadcast(GetMaxEnergyShield());
	StaggerMeterChangedDelegate.Broadcast(GetStaggerMeter());
	MaxStaggerMeterChangedDelegate.Broadcast(GetMaxStaggerMeter());
}

void UObsidianEnemyAttributesComponent::ClearGameplayTags()
{
	Super::ClearGameplayTags();
}

void UObsidianEnemyAttributesComponent::HealthChanged(const FOnAttributeChangeData& InData)
{
	const float Health = InData.NewValue;

	HealthChangedDelegate.Broadcast(Health);
}

void UObsidianEnemyAttributesComponent::MaxHealthChanged(const FOnAttributeChangeData& InData)
{
	const float MaxHealth = InData.NewValue;

	MaxHealthChangedDelegate.Broadcast(MaxHealth);
}

void UObsidianEnemyAttributesComponent::EnergyShieldChanged(const FOnAttributeChangeData& InData)
{
	const float EnergyShield = InData.NewValue;

	EnergyShieldChangedDelegate.Broadcast(EnergyShield);
}

void UObsidianEnemyAttributesComponent::MaxEnergyShieldChanged(const FOnAttributeChangeData& InData)
{
	const float MaxEnergyShield = InData.NewValue;

	MaxEnergyShieldChangedDelegate.Broadcast(MaxEnergyShield);
}

void UObsidianEnemyAttributesComponent::StaggerMeterChanged(const FOnAttributeChangeData& InData)
{
	const float StaggerMeter = InData.NewValue;
	
	StaggerMeterChangedDelegate.Broadcast(StaggerMeter);
}

void UObsidianEnemyAttributesComponent::MaxStaggerMeterChanged(const FOnAttributeChangeData& InData)
{
	const float MaxStaggerMeter = InData.NewValue;

	MaxStaggerMeterChangedDelegate.Broadcast(MaxStaggerMeter);
}

float UObsidianEnemyAttributesComponent::GetHitReactThreshold() const
{
	return (EnemyAttributeSet ? EnemyAttributeSet->GetHitReactThreshold() : 0.0f);
}

FGameplayAttribute UObsidianEnemyAttributesComponent::GetHitReactThresholdAttribute() const
{
	return (EnemyAttributeSet ? EnemyAttributeSet->GetHitReactThresholdAttribute() : nullptr);
}


// Copyright 2024 out of sCope team - Michał Ogiński

#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianEnemyAttributeSet.h"
#include "Obsidian/Obsidian.h"

UObsidianEnemyAttributesComponent::UObsidianEnemyAttributesComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EnemyAttributeSet = nullptr;
}

void UObsidianEnemyAttributesComponent::InitializeWithAbilitySystem(UObsidianAbilitySystemComponent* InASC, AActor* Owner)
{
	AActor* CurrentOwner = GetOwner();
	check(CurrentOwner);
	
	Super::InitializeWithAbilitySystem(InASC, CurrentOwner);

	EnemyAttributeSet = AbilitySystemComponent->GetSet<UObsidianEnemyAttributeSet>();
	if (!EnemyAttributeSet)
	{
		UE_LOG(LogObsidian, Error, TEXT("ObsidianEnemyAttributesComponent: Cannot initialize Attributes Component for owner [%s] with NULL Enemy Set set on the Ability System."), *GetNameSafe(Owner));
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

void UObsidianEnemyAttributesComponent::HealthChanged(const FOnAttributeChangeData& Data)
{
	const float Health = Data.NewValue;

	HealthChangedDelegate.Broadcast(Health);
}

void UObsidianEnemyAttributesComponent::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	const float MaxHealth = Data.NewValue;

	MaxHealthChangedDelegate.Broadcast(MaxHealth);
}

void UObsidianEnemyAttributesComponent::EnergyShieldChanged(const FOnAttributeChangeData& Data)
{
	const float EnergyShield = Data.NewValue;

	EnergyShieldChangedDelegate.Broadcast(EnergyShield);
}

void UObsidianEnemyAttributesComponent::MaxEnergyShieldChanged(const FOnAttributeChangeData& Data)
{
	const float MaxEnergyShield = Data.NewValue;

	MaxEnergyShieldChangedDelegate.Broadcast(MaxEnergyShield);
}

void UObsidianEnemyAttributesComponent::StaggerMeterChanged(const FOnAttributeChangeData& Data)
{
	const float StaggerMeter = Data.NewValue;
	
	StaggerMeterChangedDelegate.Broadcast(StaggerMeter);
}

void UObsidianEnemyAttributesComponent::MaxStaggerMeterChanged(const FOnAttributeChangeData& Data)
{
	const float MaxStaggerMeter = Data.NewValue;

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


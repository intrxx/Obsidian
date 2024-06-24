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
	
	EnemySpecificAttributeChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetEnemySpecificAttributeAttribute()).AddUObject(this, &ThisClass::EnemySpecificAttributeChanged);

	BroadcastInitialValues();
}

void UObsidianEnemyAttributesComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();
	
	EnemySpecificAttributeChangedDelegateHandle.Reset();
	
	EnemyAttributeSet = nullptr;
	
	Super::UninitializeFromAbilitySystem();
}

void UObsidianEnemyAttributesComponent::BroadcastInitialValues() const
{
	HealthChangedDelegate.Broadcast(GetHealth());
	MaxHealthChangedDelegate.Broadcast(GetMaxHealth());
	EnergyShieldChangedDelegate.Broadcast(GetEnergyShield());
	MaxEnergyShieldChangedDelegate.Broadcast(GetMaxEnergyShield());
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

void UObsidianEnemyAttributesComponent::EnemySpecificAttributeChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogObsidian, Warning, TEXT("Implement Enemy Specific Attribute Changed!"));
}

float UObsidianEnemyAttributesComponent::GetEnemySpecificAttribute() const
{
	return (EnemyAttributeSet ? EnemyAttributeSet->GetEnemySpecificAttribute() : 0.0f);
}

FGameplayAttribute UObsidianEnemyAttributesComponent::GetEnemySpecificAttributeAttribute() const
{
	return (EnemyAttributeSet ? EnemyAttributeSet->GetEnemySpecificAttributeAttribute() : nullptr);
}


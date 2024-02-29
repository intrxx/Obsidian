// Copyright 2024 Michał Ogiński

#include "CharacterComponents/Attributes/ObsidianAttributesComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"


UObsidianAttributesComponent::UObsidianAttributesComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	CommonAttributeSet = nullptr;
}

void UObsidianAttributesComponent::InitializeWithAbilitySystem(UObsidianAbilitySystemComponent* InASC, AActor* Owner)
{
	if(AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ObsidianAttributesComponent: Attributes Component for owner [%s] has already been initialized with an Ability System."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ObsidianAttributesComponent: Cannot initialize Attributes Component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}
	
	CommonAttributeSet = AbilitySystemComponent->GetSet<UObsidianCommonAttributeSet>();
	if (!CommonAttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("ObsidianAttributesComponent: Cannot initialize Attributes Component for owner [%s] with NULL Common Set set on the Ability System."), *GetNameSafe(Owner));
		return;
	}
	
	/** Register to listen for attribute changes */
	HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
	MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);
	ArmorChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetArmorAttribute()).AddUObject(this, &ThisClass::ArmorChanged);
	EvasionChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetEvasionAttribute()).AddUObject(this, &ThisClass::EvasionChanged);
	EnergyShieldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetEnergyShieldAttribute()).AddUObject(this, &ThisClass::EnergyShieldChanged);
	FireResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetFireResistanceAttribute()).AddUObject(this, &ThisClass::FireResistanceChanged);
	MaxFireResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxFireResistanceAttribute()).AddUObject(this, &ThisClass::MaxFireResistanceChanged);
	ColdResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetColdResistanceAttribute()).AddUObject(this, &ThisClass::ColdResistanceChanged);
	MaxColdResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxColdResistanceAttribute()).AddUObject(this, &ThisClass::MaxColdResistanceChanged);
	LightningResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetLightningResistanceAttribute()).AddUObject(this, &ThisClass::LightningResistanceChanged);
	MaxLightningResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxLightningResistanceAttribute()).AddUObject(this, &ThisClass::MaxLightningResistanceChanged);
	ChaosResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetChaosResistanceAttribute()).AddUObject(this, &ThisClass::ChaosResistanceChanged);
	MaxChaosResistanceChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxChaosResistanceAttribute()).AddUObject(this, &ThisClass::MaxChaosResistanceChanged);
	
	// Set the Health value to the MaxHealth
	AbilitySystemComponent->SetNumericAttributeBase(GetHealthAttribute(), GetMaxHealth());
}

void UObsidianAttributesComponent::UninitializeFromAbilitySystem()
{
	//TODO Clear any loose gameplay tags here
	
	HealthChangedDelegateHandle.Reset();
	MaxHealthChangedDelegateHandle.Reset();
	ArmorChangedDelegateHandle.Reset();
	EvasionChangedDelegateHandle.Reset();
	EnergyShieldChangedDelegateHandle.Reset();
	FireResistanceChangedDelegateHandle.Reset();
	MaxFireResistanceChangedDelegateHandle.Reset();
	ColdResistanceChangedDelegateHandle.Reset();
	MaxColdResistanceChangedDelegateHandle.Reset();
	LightningResistanceChangedDelegateHandle.Reset();
	MaxLightningResistanceChangedDelegateHandle.Reset();
	ChaosResistanceChangedDelegateHandle.Reset();
	MaxChaosResistanceChangedDelegateHandle.Reset();
	
	CommonAttributeSet = nullptr;
	AbilitySystemComponent = nullptr;
}

void UObsidianAttributesComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();
	
	Super::OnUnregister();
}

void UObsidianAttributesComponent::HealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Health Changed!"));
}

void UObsidianAttributesComponent::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Max Health Changed!"));
}

void UObsidianAttributesComponent::ArmorChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Armor Changed!"));
}

void UObsidianAttributesComponent::EvasionChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Evasion Changed!"));
}

void UObsidianAttributesComponent::EnergyShieldChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Energy Shield Changed!"));
}

void UObsidianAttributesComponent::FireResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Fire Resistance Changed!"));
}

void UObsidianAttributesComponent::MaxFireResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Max Fire Resistance Changed!"));
}

void UObsidianAttributesComponent::ColdResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Cold Resistance Changed!"));
}

void UObsidianAttributesComponent::MaxColdResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Max Cold Resistance Changed!"));
}

void UObsidianAttributesComponent::LightningResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Lightning Resistance Changed!"));
}

void UObsidianAttributesComponent::MaxLightningResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Max Lightning Resistance Changed!"));
}

void UObsidianAttributesComponent::ChaosResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Chaos Resistance Changed!"));
}

void UObsidianAttributesComponent::MaxChaosResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Max Chaos Resistance Changed!"));
}

float UObsidianAttributesComponent::GetHealth() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetHealth() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetHealthAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetHealthAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetMaxHealth() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetMaxHealth() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetMaxHealthAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetMaxHealthAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetArmor() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetArmor() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetArmorAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetArmorAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetEvasion() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetEvasion() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetEvasionAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetEvasionAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetEnergyShield() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetEnergyShield() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetEnergyShieldAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetEnergyShieldAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetFireResistance() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetFireResistance() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetFireResistanceAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetFireResistanceAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetMaxFireResistance() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetMaxFireResistance() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetMaxFireResistanceAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetMaxFireResistanceAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetColdResistance() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetColdResistance() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetColdResistanceAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetColdResistanceAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetMaxColdResistance() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetMaxColdResistance() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetMaxColdResistanceAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetMaxColdResistanceAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetLightningResistance() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetLightningResistance() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetLightningResistanceAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetLightningResistanceAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetMaxLightningResistance() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetMaxLightningResistance() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetMaxLightningResistanceAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetMaxLightningResistanceAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetChaosResistance() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetChaosResistance() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetChaosResistanceAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetChaosResistanceAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetMaxChaosResistance() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetMaxChaosResistance() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetMaxChaosResistanceAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetMaxChaosResistanceAttribute() : nullptr);
}





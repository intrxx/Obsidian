// Copyright 2024 out of sCope team - Michał Ogiński

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
	EnergyShieldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetEnergyShieldAttribute()).AddUObject(this, &ThisClass::EnergyShieldChanged);
	MaxEnergyShieldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxEnergyShieldAttribute()).AddUObject(this, &ThisClass::MaxEnergyShieldChanged);
	HealthRegenerationChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetHealthRegenerationAttribute()).AddUObject(this, &ThisClass::HealthRegenerationChanged);
	EnergyShieldRegenerationChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetEnergyShieldRegenerationAttribute()).AddUObject(this, &ThisClass::EnergyShieldRegenerationChanged);
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
	
	// Set the Health value to the MaxHealth // TODO Decide if I actually want to do it this way
	AbilitySystemComponent->SetNumericAttributeBase(GetHealthAttribute(), GetMaxHealth());
	AbilitySystemComponent->SetNumericAttributeBase(GetEnergyShieldAttribute(), GetMaxEnergyShield());
}

void UObsidianAttributesComponent::UninitializeFromAbilitySystem()
{
	//TODO Clear any loose gameplay tags here
	
	HealthChangedDelegateHandle.Reset();
	MaxHealthChangedDelegateHandle.Reset();
	EnergyShieldChangedDelegateHandle.Reset();
	MaxEnergyShieldChangedDelegateHandle.Reset();
	HealthRegenerationChangedDelegateHandle.Reset();
	EnergyShieldRegenerationChangedDelegateHandle.Reset();
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
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Health Changed or remove the bindingc! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Max Health Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::EnergyShieldChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Energy Shield Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::MaxEnergyShieldChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Max Energy Shield Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::HealthRegenerationChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Health Regeneration Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::EnergyShieldRegenerationChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Energy Shield Regeneration Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::ArmorChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Armor Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::EvasionChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Evasion Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::FireResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Fire Resistance Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::MaxFireResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Max Fire Resistance Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::ColdResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Cold Resistance Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::MaxColdResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Max Cold Resistance Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::LightningResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Lightning Resistance Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::MaxLightningResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Max Lightning Resistance Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::ChaosResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Chaos Resistance Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::MaxChaosResistanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Max Chaos Resistance Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::CriticalStrikeChanceChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Critical Strike Chance Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::CriticalStrikeMultiplierChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Critical Strike Multiplier Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::AttackSpeedChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Attack Speed Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::CastSpeedChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Base Class - Implement Cast Speed Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
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

float UObsidianAttributesComponent::GetEnergyShield() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetEnergyShield() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetEnergyShieldAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetEnergyShieldAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetMaxEnergyShield() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetMaxEnergyShield() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetMaxEnergyShieldAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetMaxEnergyShieldAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetHealthRegeneration() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetHealthRegeneration() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetHealthRegenerationAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetHealthRegenerationAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetEnergyShieldRegeneration() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetEnergyShieldRegeneration() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetEnergyShieldRegenerationAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetEnergyShieldRegenerationAttribute() : nullptr);
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

float UObsidianAttributesComponent::GetCriticalStrikeChance() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetCriticalStrikeChance() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetCriticalStrikeChanceAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetCriticalStrikeChanceAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetCriticalStrikeMultiplier() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetCriticalStrikeMultiplier() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetCriticalStrikeMultiplierAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetCriticalStrikeMultiplierAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetAttackSpeed() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetAttackSpeed() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetAttackSpeedAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetAttackSpeedAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetCastSpeed() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetCastSpeed() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetCastSpeedAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetCastSpeedAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetFirePenetration() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetFirePenetration() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetFirePenetrationAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetFirePenetrationAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetColdPenetration() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetColdPenetration() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetColdPenetrationAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetColdPenetrationAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetLightningPenetration() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetLightningPenetration() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetLightningPenetrationAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetLightningPenetrationAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetElementalPenetration() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetElementalPenetration() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetElementalPenetrationAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetElementalPenetrationAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetChaosPenetration() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetChaosPenetration() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetChaosPenetrationAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetChaosPenetrationAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetFireDamageMultiplier() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetFireDamageMultiplier() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetFireDamageMultiplierAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetFireDamageMultiplierAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetColdDamageMultiplier() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetColdDamageMultiplier() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetColdDamageMultiplierAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetColdDamageMultiplierAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetLightningDamageMultiplier() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetLightningDamageMultiplier() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetLightningDamageMultiplierAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetLightningDamageMultiplierAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetElementalDamageMultiplier() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetElementalDamageMultiplier() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetElementalDamageMultiplierAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetElementalDamageMultiplierAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetChaosDamageMultiplier() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetChaosDamageMultiplier() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetChaosDamageMultiplierAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetChaosDamageMultiplierAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetPhysicalDamageMultiplier() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetPhysicalDamageMultiplier() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetPhysicalDamageMultiplierAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetPhysicalDamageMultiplierAttribute() : nullptr);
}






// Copyright 2024 out of sCope team - Michał Ogiński

#include "CharacterComponents/Attributes/ObsidianAttributesComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Obsidian/Obsidian.h"
#include "Obsidian/ObsidianGameplayTags.h"


UObsidianAttributesComponent::UObsidianAttributesComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	CommonAttributeSet = nullptr;

	DeathState = EObsidianDeathState::EDS_Alive;
}

void UObsidianAttributesComponent::InitializeWithAbilitySystem(UObsidianAbilitySystemComponent* InASC, AActor* Owner)
{
	if(AbilitySystemComponent)
	{
		UE_LOG(LogObsidian, Error, TEXT("ObsidianAttributesComponent: Attributes Component for owner [%s] has already been initialized with an Ability System."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogObsidian, Error, TEXT("ObsidianAttributesComponent: Cannot initialize Attributes Component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}
	
	CommonAttributeSet = AbilitySystemComponent->GetSet<UObsidianCommonAttributeSet>();
	if (!CommonAttributeSet)
	{
		UE_LOG(LogObsidian, Error, TEXT("ObsidianAttributesComponent: Cannot initialize Attributes Component for owner [%s] with NULL Common Set set on the Ability System."), *GetNameSafe(Owner));
		return;
	}
	
	// Register to listen for attribute changes
	HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
	MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);
	EnergyShieldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetEnergyShieldAttribute()).AddUObject(this, &ThisClass::EnergyShieldChanged);
	MaxEnergyShieldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMaxEnergyShieldAttribute()).AddUObject(this, &ThisClass::MaxEnergyShieldChanged);

	CommonAttributeSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);
	
	// Set the Health value to the MaxHealth // TODO Decide if I actually want to do it this way
	AbilitySystemComponent->SetNumericAttributeBase(GetHealthAttribute(), GetMaxHealth());
	AbilitySystemComponent->SetNumericAttributeBase(GetEnergyShieldAttribute(), GetMaxEnergyShield());
}

void UObsidianAttributesComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UObsidianAttributesComponent, DeathState);
}

void UObsidianAttributesComponent::UninitializeFromAbilitySystem()
{
	HealthChangedDelegateHandle.Reset();
	MaxHealthChangedDelegateHandle.Reset();
	EnergyShieldChangedDelegateHandle.Reset();
	MaxEnergyShieldChangedDelegateHandle.Reset();

	if(CommonAttributeSet)
	{
		CommonAttributeSet->OnOutOfHealth.RemoveAll(this);
	}
	
	CommonAttributeSet = nullptr;
	AbilitySystemComponent = nullptr;
}

void UObsidianAttributesComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();
	
	Super::OnUnregister();
}

void UObsidianAttributesComponent::ClearGameplayTags()
{
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(ObsidianGameplayTags::Status_Death_Dying, 0);
		AbilitySystemComponent->RemoveLooseGameplayTag(ObsidianGameplayTags::Status_Death_Dead, 0);
	}
}

void UObsidianAttributesComponent::StartDeath()
{
	if(DeathState != EObsidianDeathState::EDS_Alive)
	{
		return;
	}

	DeathState = EObsidianDeathState::EDS_DeathStarted;

	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(ObsidianGameplayTags::Status_Death_Dying, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);
	
	OnDeathStarted.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void UObsidianAttributesComponent::FinishDeath()
{
	if(DeathState != EObsidianDeathState::EDS_DeathStarted)
	{
		return;
	}

	DeathState = EObsidianDeathState::EDS_DeathFinished;

	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(ObsidianGameplayTags::Status_Death_Dead, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void UObsidianAttributesComponent::OnRep_DeathState(EObsidianDeathState OldDeathState)
{
	const EObsidianDeathState NewDeathState = DeathState;

	// Revert the death state for now since we rely on StartDeath and FinishDeath to change it.
	DeathState = OldDeathState;

	if (OldDeathState > NewDeathState)
	{
		// The server is trying to set us back, but we've already predicted past the server state.
		UE_LOG(LogObsidian, Warning, TEXT("ObsidianAttributesComp: Predicted past server death state [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		return;
	}

	if (OldDeathState == EObsidianDeathState::EDS_Alive)
	{
		if (NewDeathState == EObsidianDeathState::EDS_DeathStarted)
		{
			StartDeath();
		}
		else if (NewDeathState == EObsidianDeathState::EDS_DeathFinished)
		{
			StartDeath();
			FinishDeath();
		}
		else
		{
			UE_LOG(LogObsidian, Error, TEXT("ObsidianAttributesComp: Invalid death transition [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}
	}
	else if (OldDeathState == EObsidianDeathState::EDS_DeathStarted)
	{
		if (NewDeathState == EObsidianDeathState::EDS_DeathFinished)
		{
			FinishDeath();
		}
		else
		{
			UE_LOG(LogObsidian, Error, TEXT("ObsidianAttributesComp: Invalid death transition [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}
	}

	ensureMsgf((DeathState == NewDeathState), TEXT("ObsidianAttributesComp: Death transition failed [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
#if WITH_SERVER_CODE
	if(AbilitySystemComponent && DamageEffectSpec)
	{
		FGameplayEventData Payload;
		Payload.EventTag = ObsidianGameplayTags::GameplayEvent_Death;
		Payload.Instigator = DamageInstigator;
		Payload.Target = AbilitySystemComponent->GetAvatarActor();
		Payload.OptionalObject = DamageEffectSpec->Def;
		Payload.ContextHandle = DamageEffectSpec->GetEffectContext();
		Payload.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
		Payload.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
		Payload.EventMagnitude = DamageMagnitude;

		FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
		AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
	}
#endif 
}

void UObsidianAttributesComponent::HealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogObsidian, Warning, TEXT("Base Class - Implement Health Changed or remove the bindingc! - For %s"), *GetNameSafe(GetOwner()));
	UE_LOG(LogObsidian, Warning, TEXT("New life: %f"), Data.NewValue);
}

void UObsidianAttributesComponent::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogObsidian, Warning, TEXT("Base Class - Implement Max Health Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::EnergyShieldChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogObsidian, Warning, TEXT("Base Class - Implement Energy Shield Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
}

void UObsidianAttributesComponent::MaxEnergyShieldChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogObsidian, Warning, TEXT("Base Class - Implement Max Energy Shield Changed or remove the binding! - For %s"), *GetNameSafe(GetOwner()));
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

float UObsidianAttributesComponent::GetSpellSuppressionChance() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetSpellSuppressionChance() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetSpellSuppressionChanceAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetSpellSuppressionChanceAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetSpellSuppressionMagnitude() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetSpellSuppressionMagnitude() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetSpellSuppressionMagnitudeAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetSpellSuppressionMagnitudeAttribute() : nullptr);
}

float UObsidianAttributesComponent::GetAllElementalResistances() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetAllElementalResistances() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetAllElementalResistancesAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetAllElementalResistancesAttribute() : nullptr);
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

float UObsidianAttributesComponent::GetAllElementalPenetration() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetAllElementalPenetration() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetAllElementalPenetrationAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetAllElementalPenetrationAttribute() : nullptr);
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

float UObsidianAttributesComponent::GetAllElementalDamageMultiplier() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetAllElementalDamageMultiplier() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetAllElementalDamageMultiplierAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetAllElementalDamageMultiplierAttribute() : nullptr);
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

float UObsidianAttributesComponent::GetMovementSpeed() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetMovementSpeed() : 0.0f);
}

FGameplayAttribute UObsidianAttributesComponent::GetMovementSpeedAttribute() const
{
	return (CommonAttributeSet ? CommonAttributeSet->GetMovementSpeedAttribute() : nullptr);
}






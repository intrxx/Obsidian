// Copyright 2026 out of sCope team - intrxx

#include "UI/WidgetControllers/ObMainOverlayWidgetController.h"

#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "Core/FunctionLibraries/ObsidianUIFunctionLibrary.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "Obsidian/ObsidianLogCategories.h"


void UObMainOverlayWidgetController::OnWidgetControllerSetupCompleted()
{
	check(OwnerPlayerState.IsValid());
	check(OwnerPlayerController.IsValid());
	AObsidianPlayerController* PlayerController = OwnerPlayerController.Get();
	if (PlayerController == nullptr)
	{
		UE_LOG(ObLogUIMainOverlay, Error, TEXT("PlayerController is invalid in [%hs]."),
			__FUNCTION__);
		return;
	}

	PlayerController->OnEnemyActorHoveredDelegate.BindDynamic(this, &ThisClass::UpdateHoveringOverTarget);
	PlayerController->OnBossDetectedPlayerDelegate.BindDynamic(this, &ThisClass::UpdateBossDetectionInfo);

	OwnerAttributesComponent = UObsidianHeroAttributesComponent::FindHeroAttributesComponent(
					PlayerController->GetPawn());
	check(OwnerAttributesComponent.IsValid());

	OwnerAbilitySystemComponent = PlayerController->GetObsidianAbilitySystemComponent();
	check(OwnerAbilitySystemComponent.IsValid());
	if (UObsidianAbilitySystemComponent* ObsidianASC = OwnerAbilitySystemComponent.Get())
	{
		HandleBindingCallbacks(ObsidianASC);
	
		ObsidianASC->OnEffectAppliedAssetTags.AddUObject(this, &ThisClass::HandleEffectApplied);
		ObsidianASC->OnAuraDisabledDelegate.BindDynamic(this, &ThisClass::DestroyAuraWidget);
	}
	
	SetInitialAttributeValues();
}

FObsidianSpecialResourceVisuals UObMainOverlayWidgetController::GetSpecialResourceVisuals() const
{
	UObsidianHeroAttributesComponent* HeroAttributesComp = OwnerAttributesComponent.Get();
	if (HeroAttributesComp == nullptr)
	{
		if (OwnerPlayerController.IsValid())
		{
			HeroAttributesComp = UObsidianHeroAttributesComponent::FindHeroAttributesComponent(
					OwnerPlayerController.Get()->GetPawn());
			if (HeroAttributesComp == nullptr)
			{
				UE_LOG(ObLogUIMainOverlay, Error, TEXT("HeroAttributesComp is invalid in [%hs]."),
					__FUNCTION__);
				return FObsidianSpecialResourceVisuals();
			}
		}
	}
	
	return	HeroAttributesComp->GetSpecialResourceVisuals();
}

void UObMainOverlayWidgetController::HandleBindingCallbacks(UObsidianAbilitySystemComponent* InObsidianASC)
{
	if(InObsidianASC == nullptr)
	{
		UE_LOG(ObLogUIMainOverlay, Error, TEXT("ObsidianASC is invalid in [%hs]."), __FUNCTION__);
		return;
	}
	
	const UObsidianHeroAttributesComponent* HeroAttributesComp = OwnerAttributesComponent.Get();
	if (HeroAttributesComp == nullptr)
	{
		UE_LOG(ObLogUIMainOverlay, Error, TEXT("HeroAttributesComp is invalid in [%hs]."),
			__FUNCTION__);
		return;
	}
	
	/** Hero Set */
	ManaChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetManaAttribute()).AddUObject(this, &ThisClass::ManaChanged);
	MaxManaChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxManaAttribute()).AddUObject(this, &ThisClass::MaxManaChanged);
	SpecialResourceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetSpecialResourceAttribute()).AddUObject(this, &ThisClass::SpecialResourceChanged);
	MaxSpecialResourceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxSpecialResourceAttribute()).AddUObject(this, &ThisClass::MaxSpecialResourceChanged);
	ExperienceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetExperienceAttribute()).AddUObject(this, &ThisClass::ExperienceChanged);
	MaxExperienceChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxExperienceAttribute()).AddUObject(this, &ThisClass::MaxExperienceChanged);
	PassiveSkillPointsChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetPassiveSkillPointsAttribute()).AddUObject(this, &ThisClass::PassiveSkillPointsChanged);
	AscensionPointsChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetAscensionPointsAttribute()).AddUObject(this, &ThisClass::AscensionPointsChanged);
	OnStaminaChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetStaminaAttribute()).AddUObject(this, &ThisClass::StaminaChanged);
	OnMaxStaminaChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxStaminaAttribute()).AddUObject(this, &ThisClass::MaxStaminaChanged);
	
	/** Common Set */
	HealthChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
	MaxHealthChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);
	EnergyShieldChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetEnergyShieldAttribute()).AddUObject(this, &ThisClass::EnergyShieldChanged);
	MaxEnergyShieldChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxEnergyShieldAttribute()).AddUObject(this, &ThisClass::MaxEnergyShieldChanged);
	StaggerMeterChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetStaggerMeterAttribute()).AddUObject(this, &ThisClass::StaggerMeterChanged);
	MaxStaggerMeterChangedDelegateHandle = InObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxStaggerMeterAttribute()).AddUObject(this, &ThisClass::MaxStaggerMeterChanged);
}

void UObMainOverlayWidgetController::SetInitialAttributeValues() const
{
	const UObsidianHeroAttributesComponent* HeroAttributesComp = OwnerAttributesComponent.Get();
	if (HeroAttributesComp == nullptr)
	{
		UE_LOG(ObLogUIMainOverlay, Error, TEXT("HeroAttributesComp is invalid in [%hs]."),
			__FUNCTION__);
		return;
	}
	
	OnHealthChangedDelegate.Broadcast(HeroAttributesComp->GetHealth());
	OnMaxHealthChangedDelegate.Broadcast(HeroAttributesComp->GetMaxHealth());
	OnManaChangedDelegate.Broadcast(HeroAttributesComp->GetMana());
	OnMaxManaChangedDelegate.Broadcast(HeroAttributesComp->GetMaxMana());
	OnEnergyShieldChangedDelegate.Broadcast(HeroAttributesComp->GetEnergyShield());
	OnMaxEnergyShieldChangedDelegate.Broadcast(HeroAttributesComp->GetMaxEnergyShield());
	OnSpecialResourceChangedDelegate.Broadcast(HeroAttributesComp->GetSpecialResource());
	OnMaxSpecialResourceChangedDelegate.Broadcast(HeroAttributesComp->GetMaxSpecialResource());
	OnStaggerMeterChangedDelegate.Broadcast(HeroAttributesComp->GetStaggerMeter());
	OnMaxStaggerMeterChangedDelegate.Broadcast(HeroAttributesComp->GetMaxStaggerMeter());
	OnPassiveSkillPointsChangedDelegate.Broadcast(HeroAttributesComp->GetPassiveSkillPoints());
	OnAscensionPointsChangedDelegate.Broadcast(HeroAttributesComp->GetAscensionPoints());
	OnStaminaChangedDelegate.Broadcast(HeroAttributesComp->GetStamina());
	OnMaxStaminaChangedDelegate.Broadcast(HeroAttributesComp->GetMaxStamina());
}

void UObMainOverlayWidgetController::SetInitialStaggerMeter() const
{
	UObsidianHeroAttributesComponent* HeroAttributesComp = OwnerAttributesComponent.Get();
	if (HeroAttributesComp == nullptr)
	{
		if (OwnerPlayerController.IsValid())
		{
			HeroAttributesComp = UObsidianHeroAttributesComponent::FindHeroAttributesComponent(
					OwnerPlayerController.Get()->GetPawn());
			if (HeroAttributesComp == nullptr)
			{
				UE_LOG(ObLogUIMainOverlay, Error, TEXT("HeroAttributesComp is invalid in [%hs]."),
					__FUNCTION__);
				return;
			}
		}
	}
	
	OnStaggerMeterChangedDelegate.Broadcast(HeroAttributesComp->GetStaggerMeter());
	OnMaxStaggerMeterChangedDelegate.Broadcast(HeroAttributesComp->GetMaxStaggerMeter());
}

void UObMainOverlayWidgetController::SetInitialExperienceValues()
{
	UObsidianHeroAttributesComponent* HeroAttributesComp = OwnerAttributesComponent.Get();
	if (HeroAttributesComp == nullptr)
	{
		if (OwnerPlayerController.IsValid())
		{
			HeroAttributesComp = UObsidianHeroAttributesComponent::FindHeroAttributesComponent(
					OwnerPlayerController.Get()->GetPawn());
			if (HeroAttributesComp == nullptr)
			{
				UE_LOG(ObLogUIMainOverlay, Error, TEXT("HeroAttributesComp is invalid in [%hs]."),
					__FUNCTION__);
				return;
			}
		}
	}
	
	if (AObsidianPlayerState* PlayerState = OwnerPlayerState.Get())
	{
		const uint8 LastHeroLevel = PlayerState->GetHeroLevel() - 1;
		if (LastHeroLevel > 1)
		{
			MaxExperienceOldValue = UObsidianHeroAttributeSet::GetMaxExperienceForLevel(LastHeroLevel);
		}
	
		OnExperienceChangedDelegate.Execute(HeroAttributesComp->GetExperience());
		OnMaxExperienceChangedDelegate.Execute(HeroAttributesComp->GetMaxExperience(), MaxExperienceOldValue);
	}
}

void UObMainOverlayWidgetController::SetInitialStaminaValues()
{
	UObsidianHeroAttributesComponent* HeroAttributesComp = OwnerAttributesComponent.Get();
	if (HeroAttributesComp == nullptr)
	{
		if (OwnerPlayerController.IsValid())
		{
			HeroAttributesComp = UObsidianHeroAttributesComponent::FindHeroAttributesComponent(
					OwnerPlayerController.Get()->GetPawn());
			if (HeroAttributesComp == nullptr)
			{
				UE_LOG(ObLogUIMainOverlay, Error, TEXT("HeroAttributesComp is invalid in [%hs]."),
					__FUNCTION__);
				return;
			}
		}
	}
	
	if (HeroAttributesComp)
	{
		OnStaminaChangedDelegate.Broadcast(HeroAttributesComp->GetStamina());
		OnMaxStaminaChangedDelegate.Broadcast(HeroAttributesComp->GetMaxStamina());
	}
}

void UObMainOverlayWidgetController::HandleEffectApplied(const FObsidianEffectUIData& InUIData)
{
	for(const FGameplayTag& Tag : InUIData.AssetTags)
	{
		const FGameplayTag EffectUIDataTag = FGameplayTag::RequestGameplayTag(FName("UI.EffectData"));
		if(Tag.MatchesTag(EffectUIDataTag))
		{
			FObsidianEffectUIDataWidgetRow* Row = UObsidianUIFunctionLibrary::GetDataTableRowByTag<
				FObsidianEffectUIDataWidgetRow>(UIEffectDataWidgetTable, Tag);
			Row->EffectDuration = InUIData.EffectDuration;
					
			if(InUIData.bStackingEffect)
			{
				EffectStackingUIDataDelegate.Broadcast(*Row, InUIData.StackingData);
			}
			else
			{
				EffectUIDataWidgetRowDelegate.Broadcast(*Row);
			}
		}

		if(InUIData.EffectDurationPolicy == EGameplayEffectDurationType::HasDuration)
		{
			const FGameplayTag HealthGlobeDataTag = ObsidianGameplayTags::UI::GlobeData::HealingHealth;
			const FGameplayTag ManaGlobeDataTag = ObsidianGameplayTags::UI::GlobeData::ReplenishingMana;
			if(Tag.MatchesTag(HealthGlobeDataTag))
			{
				EffectUIHealthGlobeDataDelegate.Broadcast(InUIData.EffectDuration, InUIData.EffectMagnitude);
			}
			if(Tag.MatchesTag(ManaGlobeDataTag))
			{
				EffectUIManaGlobeDataDelegate.Broadcast(InUIData.EffectDuration, InUIData.EffectMagnitude);
			}
		}
	}
}

void UObMainOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& InData) const
{
	OnManaChangedDelegate.Broadcast(InData.NewValue);
}

void UObMainOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& InData) const
{
	OnMaxManaChangedDelegate.Broadcast(InData.NewValue);
}

void UObMainOverlayWidgetController::SpecialResourceChanged(const FOnAttributeChangeData& InData) const
{
	OnSpecialResourceChangedDelegate.Broadcast(InData.NewValue);
}

void UObMainOverlayWidgetController::MaxSpecialResourceChanged(const FOnAttributeChangeData& InData) const
{
	OnMaxSpecialResourceChangedDelegate.Broadcast(InData.NewValue);
}

void UObMainOverlayWidgetController::ExperienceChanged(const FOnAttributeChangeData& InData) const
{
	if(OnExperienceChangedDelegate.IsBound())
	{
		OnExperienceChangedDelegate.Execute(InData.NewValue);
	}
}

void UObMainOverlayWidgetController::MaxExperienceChanged(const FOnAttributeChangeData& InData)
{
	if(OnMaxExperienceChangedDelegate.IsBound())
	{
		MaxExperienceOldValue = InData.OldValue;
		OnMaxExperienceChangedDelegate.Execute(InData.NewValue, MaxExperienceOldValue);
	}
}

void UObMainOverlayWidgetController::PassiveSkillPointsChanged(const FOnAttributeChangeData& InData) const
{
	OnPassiveSkillPointsChangedDelegate.Broadcast(InData.NewValue);
}

void UObMainOverlayWidgetController::AscensionPointsChanged(const FOnAttributeChangeData& InData) const
{
	OnAscensionPointsChangedDelegate.Broadcast(InData.NewValue);
}

void UObMainOverlayWidgetController::StaminaChanged(const FOnAttributeChangeData& InData) const
{
	OnStaminaChangedDelegate.Broadcast(InData.NewValue);
}

void UObMainOverlayWidgetController::MaxStaminaChanged(const FOnAttributeChangeData& InData) const
{
	OnMaxStaminaChangedDelegate.Broadcast(InData.NewValue);
}

void UObMainOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& InData) const
{
	OnHealthChangedDelegate.Broadcast(InData.NewValue);
}

void UObMainOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& InData) const
{
	OnMaxHealthChangedDelegate.Broadcast(InData.NewValue);
}

void UObMainOverlayWidgetController::EnergyShieldChanged(const FOnAttributeChangeData& InData) const
{
	OnEnergyShieldChangedDelegate.Broadcast(InData.NewValue);
}

void UObMainOverlayWidgetController::MaxEnergyShieldChanged(const FOnAttributeChangeData& InData) const
{
	OnMaxEnergyShieldChangedDelegate.Broadcast(InData.NewValue);
}

void UObMainOverlayWidgetController::StaggerMeterChanged(const FOnAttributeChangeData& InData) const
{
	OnStaggerMeterChangedDelegate.Broadcast(InData.NewValue);
}

void UObMainOverlayWidgetController::MaxStaggerMeterChanged(const FOnAttributeChangeData& InData) const
{
	OnMaxStaggerMeterChangedDelegate.Broadcast(InData.NewValue);
}

void UObMainOverlayWidgetController::UpdateHoveringOverTarget(AActor* InTargetActor, const bool bInHoveredOver)
{
	OnUpdateRegularEnemyTargetForHealthBarDelegate.Broadcast(InTargetActor, bInHoveredOver);
}

void UObMainOverlayWidgetController::UpdateBossDetectionInfo(AActor* InBossActor, const bool bInSeen)
{
	OnUpdateBossEnemyTargetForHealthBarDelegate.Broadcast(InBossActor, bInSeen);
}

void UObMainOverlayWidgetController::DestroyAuraWidget(const FGameplayTag InAuraWidgetTag)
{
	if(OnAuraWidgetDestructionInfoReceivedDelegate.IsBound())
	{
		OnAuraWidgetDestructionInfoReceivedDelegate.Execute(InAuraWidgetTag);
	}
}

void UObMainOverlayWidgetController::UpdateHealthInfoGlobe(const float InMagnitude) const
{
	EffectUIHealthGlobeDataDelegate.Broadcast(0, InMagnitude);
}

void UObMainOverlayWidgetController::UpdateManaInfoGlobe(const float InMagnitude) const 
{
	EffectUIManaGlobeDataDelegate.Broadcast(0, InMagnitude);
}




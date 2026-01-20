// Copyright 2026 out of sCope team - intrxx

#include "UI/WidgetControllers/ObMainOverlayWidgetController.h"


#include "Core/FunctionLibraries/ObsidianUIFunctionLibrary.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "Obsidian/ObsidianGameplayTags.h"

DEFINE_LOG_CATEGORY(LogWidgetController_MainOverlay)

void UObMainOverlayWidgetController::OnWidgetControllerSetupCompleted()
{
	check(OwnerPlayerState.IsValid());
	check(OwnerPlayerController.IsValid());
	AObsidianPlayerController* PlayerController = OwnerPlayerController.Get();
	if (PlayerController == nullptr)
	{
		UE_LOG(LogWidgetController_MainOverlay, Error, TEXT("PlayerController is invalid in [%hs]."),
			__FUNCTION__);
		return;
	}

	PlayerController->OnEnemyActorHoveredDelegate.BindDynamic(this, &ThisClass::UpdateHoveringOverTarget);
	PlayerController->OnBossDetectedPlayerDelegate.BindDynamic(this, &ThisClass::UpdateBossDetectionInfo);

	OwnerAttributesComponent = UObsidianHeroAttributesComponent::FindAttributesComponent(
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
			HeroAttributesComp = UObsidianHeroAttributesComponent::FindAttributesComponent(
					OwnerPlayerController.Get()->GetPawn());
			if (HeroAttributesComp == nullptr)
			{
				UE_LOG(LogWidgetController_MainOverlay, Error, TEXT("HeroAttributesComp is invalid in [%hs]."),
					__FUNCTION__);
				return FObsidianSpecialResourceVisuals();
			}
		}
	}
	
	return	HeroAttributesComp->GetSpecialResourceVisuals();
}

void UObMainOverlayWidgetController::HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC)
{
	if(ObsidianASC == nullptr)
	{
		UE_LOG(LogWidgetController_MainOverlay, Error, TEXT("ObsidianASC is invalid in [%hs]."), __FUNCTION__);
		return;
	}
	
	const UObsidianHeroAttributesComponent* HeroAttributesComp = OwnerAttributesComponent.Get();
	if (HeroAttributesComp == nullptr)
	{
		UE_LOG(LogWidgetController_MainOverlay, Error, TEXT("HeroAttributesComp is invalid in [%hs]."),
			__FUNCTION__);
		return;
	}
	
	/** Hero Set */
	ManaChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetManaAttribute()).AddUObject(this, &ThisClass::ManaChanged);
	MaxManaChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxManaAttribute()).AddUObject(this, &ThisClass::MaxManaChanged);
	SpecialResourceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetSpecialResourceAttribute()).AddUObject(this, &ThisClass::SpecialResourceChanged);
	MaxSpecialResourceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxSpecialResourceAttribute()).AddUObject(this, &ThisClass::MaxSpecialResourceChanged);
	ExperienceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetExperienceAttribute()).AddUObject(this, &ThisClass::ExperienceChanged);
	MaxExperienceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxExperienceAttribute()).AddUObject(this, &ThisClass::MaxExperienceChanged);
	PassiveSkillPointsChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetPassiveSkillPointsAttribute()).AddUObject(this, &ThisClass::PassiveSkillPointsChanged);
	AscensionPointsChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetAscensionPointsAttribute()).AddUObject(this, &ThisClass::AscensionPointsChanged);
		
	/** Common Set */
	HealthChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
	MaxHealthChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);
	EnergyShieldChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetEnergyShieldAttribute()).AddUObject(this, &ThisClass::EnergyShieldChanged);
	MaxEnergyShieldChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxEnergyShieldAttribute()).AddUObject(this, &ThisClass::MaxEnergyShieldChanged);
	StaggerMeterChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetStaggerMeterAttribute()).AddUObject(this, &ThisClass::StaggerMeterChanged);
	MaxStaggerMeterChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(
		HeroAttributesComp->GetMaxStaggerMeterAttribute()).AddUObject(this, &ThisClass::MaxStaggerMeterChanged);
}

void UObMainOverlayWidgetController::SetInitialAttributeValues() const
{
	const UObsidianHeroAttributesComponent* HeroAttributesComp = OwnerAttributesComponent.Get();
	if (HeroAttributesComp == nullptr)
	{
		UE_LOG(LogWidgetController_MainOverlay, Error, TEXT("HeroAttributesComp is invalid in [%hs]."),
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
}

void UObMainOverlayWidgetController::SetInitialStaggerMeter() const
{
	UObsidianHeroAttributesComponent* HeroAttributesComp = OwnerAttributesComponent.Get();
	if (HeroAttributesComp == nullptr)
	{
		if (OwnerPlayerController.IsValid())
		{
			HeroAttributesComp = UObsidianHeroAttributesComponent::FindAttributesComponent(
					OwnerPlayerController.Get()->GetPawn());
			if (HeroAttributesComp == nullptr)
			{
				UE_LOG(LogWidgetController_MainOverlay, Error, TEXT("HeroAttributesComp is invalid in [%hs]."),
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
			HeroAttributesComp = UObsidianHeroAttributesComponent::FindAttributesComponent(
					OwnerPlayerController.Get()->GetPawn());
			if (HeroAttributesComp == nullptr)
			{
				UE_LOG(LogWidgetController_MainOverlay, Error, TEXT("HeroAttributesComp is invalid in [%hs]."),
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

void UObMainOverlayWidgetController::HandleEffectApplied(const FObsidianEffectUIData& UIData)
{
	for(const FGameplayTag& Tag : UIData.AssetTags)
	{
		const FGameplayTag EffectUIDataTag = FGameplayTag::RequestGameplayTag(FName("UI.EffectData"));
		if(Tag.MatchesTag(EffectUIDataTag))
		{
			FObsidianEffectUIDataWidgetRow* Row = UObsidianUIFunctionLibrary::GetDataTableRowByTag<
				FObsidianEffectUIDataWidgetRow>(UIEffectDataWidgetTable, Tag);
			Row->EffectDuration = UIData.EffectDuration;
					
			if(UIData.bStackingEffect)
			{
				EffectStackingUIDataDelegate.Broadcast(*Row, UIData.StackingData);
			}
			else
			{
				EffectUIDataWidgetRowDelegate.Broadcast(*Row);
			}
		}

		if(UIData.EffectDurationPolicy == EGameplayEffectDurationType::HasDuration)
		{
			const FGameplayTag HealthGlobeDataTag = ObsidianGameplayTags::UI_GlobeData_HealingHealth;
			const FGameplayTag ManaGlobeDataTag = ObsidianGameplayTags::UI_GlobeData_RepleanishingMana;
			if(Tag.MatchesTag(HealthGlobeDataTag))
			{
				EffectUIHealthGlobeDataDelegate.Broadcast(UIData.EffectDuration, UIData.EffectMagnitude);
			}
			if(Tag.MatchesTag(ManaGlobeDataTag))
			{
				EffectUIManaGlobeDataDelegate.Broadcast(UIData.EffectDuration, UIData.EffectMagnitude);
			}
		}
	}
}

void UObMainOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChangedDelegate.Broadcast(Data.NewValue);
}

void UObMainOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChangedDelegate.Broadcast(Data.NewValue);
}

void UObMainOverlayWidgetController::SpecialResourceChanged(const FOnAttributeChangeData& Data) const
{
	OnSpecialResourceChangedDelegate.Broadcast(Data.NewValue);
}

void UObMainOverlayWidgetController::MaxSpecialResourceChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxSpecialResourceChangedDelegate.Broadcast(Data.NewValue);
}

void UObMainOverlayWidgetController::ExperienceChanged(const FOnAttributeChangeData& Data) const
{
	if(OnExperienceChangedDelegate.IsBound())
	{
		OnExperienceChangedDelegate.Execute(Data.NewValue);
	}
}

void UObMainOverlayWidgetController::MaxExperienceChanged(const FOnAttributeChangeData& Data)
{
	if(OnMaxExperienceChangedDelegate.IsBound())
	{
		MaxExperienceOldValue = Data.OldValue;
		OnMaxExperienceChangedDelegate.Execute(Data.NewValue, MaxExperienceOldValue);
	}
}

void UObMainOverlayWidgetController::PassiveSkillPointsChanged(const FOnAttributeChangeData& Data) const
{
	OnPassiveSkillPointsChangedDelegate.Broadcast(Data.NewValue);
}

void UObMainOverlayWidgetController::AscensionPointsChanged(const FOnAttributeChangeData& Data) const
{
	OnAscensionPointsChangedDelegate.Broadcast(Data.NewValue);
}

void UObMainOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChangedDelegate.Broadcast(Data.NewValue);
}

void UObMainOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChangedDelegate.Broadcast(Data.NewValue);
}

void UObMainOverlayWidgetController::EnergyShieldChanged(const FOnAttributeChangeData& Data) const
{
	OnEnergyShieldChangedDelegate.Broadcast(Data.NewValue);
}

void UObMainOverlayWidgetController::MaxEnergyShieldChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxEnergyShieldChangedDelegate.Broadcast(Data.NewValue);
}

void UObMainOverlayWidgetController::StaggerMeterChanged(const FOnAttributeChangeData& Data) const
{
	OnStaggerMeterChangedDelegate.Broadcast(Data.NewValue);
}

void UObMainOverlayWidgetController::MaxStaggerMeterChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxStaggerMeterChangedDelegate.Broadcast(Data.NewValue);
}

void UObMainOverlayWidgetController::UpdateHoveringOverTarget(AActor* TargetActor, const bool bHoveredOver)
{
	OnUpdateRegularEnemyTargetForHealthBarDelegate.Broadcast(TargetActor, bHoveredOver);
}

void UObMainOverlayWidgetController::UpdateBossDetectionInfo(AActor* BossActor, const bool bSeen)
{
	OnUpdateBossEnemyTargetForHealthBarDelegate.Broadcast(BossActor, bSeen);
}

void UObMainOverlayWidgetController::DestroyAuraWidget(const FGameplayTag AuraWidgetTag)
{
	if(OnAuraWidgetDestructionInfoReceivedDelegate.IsBound())
	{
		OnAuraWidgetDestructionInfoReceivedDelegate.Execute(AuraWidgetTag);
	}
}

void UObMainOverlayWidgetController::UpdateHealthInfoGlobe(const float Magnitude) const
{
	EffectUIHealthGlobeDataDelegate.Broadcast(0, Magnitude);
}

void UObMainOverlayWidgetController::UpdateManaInfoGlobe(const float Magnitude) const 
{
	EffectUIManaGlobeDataDelegate.Broadcast(0, Magnitude);
}




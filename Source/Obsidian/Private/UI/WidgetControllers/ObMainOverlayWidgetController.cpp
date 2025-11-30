// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/WidgetControllers/ObMainOverlayWidgetController.h"

// ~ Core
#include "Core/FunctionLibraries/ObsidianUIFunctionLibrary.h"

// ~ Project
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "Obsidian/ObsidianGameplayTags.h"

void UObMainOverlayWidgetController::OnWidgetControllerSetupCompleted()
{
	check(ObsidianAbilitySystemComponent);
	check(AttributesComponent);
	check(ObsidianPlayerController);
	check(ObsidianPlayerState);
	
	HandleBindingCallbacks(ObsidianAbilitySystemComponent);
	
	ObsidianPlayerController->OnEnemyActorHoveredDelegate.BindDynamic(this, &ThisClass::UpdateHoveringOverTarget);
	ObsidianPlayerController->OnBossDetectedPlayerDelegate.BindDynamic(this, &ThisClass::UpdateBossDetectionInfo);
	
	ObsidianAbilitySystemComponent->OnEffectAppliedAssetTags.AddUObject(this, &ThisClass::HandleEffectApplied);
	
	SetInitialAttributeValues();

	ObsidianAbilitySystemComponent->OnAuraDisabledDelegate.BindDynamic(this, &ThisClass::DestroyAuraWidget);
}

FObsidianSpecialResourceVisuals UObMainOverlayWidgetController::GetSpecialResourceVisuals() const
{
	if(AttributesComponent)
	{
		return	AttributesComponent->GetSpecialResourceVisuals();
	}
	return FObsidianSpecialResourceVisuals();
}

void UObMainOverlayWidgetController::HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC)
{
	/** Hero Set */
	ManaChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetManaAttribute()).AddUObject(this, &ThisClass::ManaChanged);
	MaxManaChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxManaAttribute()).AddUObject(this, &ThisClass::MaxManaChanged);
	SpecialResourceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetSpecialResourceAttribute()).AddUObject(this, &ThisClass::SpecialResourceChanged);
	MaxSpecialResourceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxSpecialResourceAttribute()).AddUObject(this, &ThisClass::MaxSpecialResourceChanged);
	ExperienceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetExperienceAttribute()).AddUObject(this, &ThisClass::ExperienceChanged);
	MaxExperienceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxExperienceAttribute()).AddUObject(this, &ThisClass::MaxExperienceChanged);
	PassiveSkillPointsChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetPassiveSkillPointsAttribute()).AddUObject(this, &ThisClass::PassiveSkillPointsChanged);
	AscensionPointsChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetAscensionPointsAttribute()).AddUObject(this, &ThisClass::AscensionPointsChanged);
		
	/** Common Set */
	HealthChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
	MaxHealthChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);
	EnergyShieldChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetEnergyShieldAttribute()).AddUObject(this, &ThisClass::EnergyShieldChanged);
	MaxEnergyShieldChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxEnergyShieldAttribute()).AddUObject(this, &ThisClass::MaxEnergyShieldChanged);
	StaggerMeterChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetStaggerMeterAttribute()).AddUObject(this, &ThisClass::StaggerMeterChanged);
	MaxStaggerMeterChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxStaggerMeterAttribute()).AddUObject(this, &ThisClass::MaxStaggerMeterChanged);
}

void UObMainOverlayWidgetController::SetInitialAttributeValues() const
{
	OnHealthChangedDelegate.Broadcast(AttributesComponent->GetHealth());
	OnMaxHealthChangedDelegate.Broadcast(AttributesComponent->GetMaxHealth());
	OnManaChangedDelegate.Broadcast(AttributesComponent->GetMana());
	OnMaxManaChangedDelegate.Broadcast(AttributesComponent->GetMaxMana());
	OnEnergyShieldChangedDelegate.Broadcast(AttributesComponent->GetEnergyShield());
	OnMaxEnergyShieldChangedDelegate.Broadcast(AttributesComponent->GetMaxEnergyShield());
	OnSpecialResourceChangedDelegate.Broadcast(AttributesComponent->GetSpecialResource());
	OnMaxSpecialResourceChangedDelegate.Broadcast(AttributesComponent->GetMaxSpecialResource());
	OnStaggerMeterChangedDelegate.Broadcast(AttributesComponent->GetStaggerMeter());
	OnMaxStaggerMeterChangedDelegate.Broadcast(AttributesComponent->GetMaxStaggerMeter());
	OnPassiveSkillPointsChangedDelegate.Broadcast(AttributesComponent->GetPassiveSkillPoints());
	OnAscensionPointsChangedDelegate.Broadcast(AttributesComponent->GetAscensionPoints());
}

void UObMainOverlayWidgetController::SetInitialStaggerMeter() const
{
	OnStaggerMeterChangedDelegate.Broadcast(AttributesComponent->GetStaggerMeter());
	OnMaxStaggerMeterChangedDelegate.Broadcast(AttributesComponent->GetMaxStaggerMeter());
}

void UObMainOverlayWidgetController::SetInitialExperienceValues()
{
	const uint8 LastHeroLevel = ObsidianPlayerState->GetHeroLevel() - 1;
	if (ObsidianPlayerState && LastHeroLevel > 1)
	{
		MaxExperienceOldValue = UObsidianHeroAttributeSet::GetMaxExperienceForLevel(LastHeroLevel);
	}
	
	OnExperienceChangedDelegate.Execute(AttributesComponent->GetExperience());
	OnMaxExperienceChangedDelegate.Execute(AttributesComponent->GetMaxExperience(), MaxExperienceOldValue);
}

void UObMainOverlayWidgetController::HandleEffectApplied(const FObsidianEffectUIData& UIData)
{
	for(const FGameplayTag& Tag : UIData.AssetTags)
	{
		const FGameplayTag EffectUIDataTag = FGameplayTag::RequestGameplayTag(FName("UI.EffectData"));
		if(Tag.MatchesTag(EffectUIDataTag))
		{
			FObsidianEffectUIDataWidgetRow* Row = UObsidianUIFunctionLibrary::GetDataTableRowByTag<FObsidianEffectUIDataWidgetRow>(UIEffectDataWidgetTable, Tag);
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




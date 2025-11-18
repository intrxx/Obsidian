// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/WidgetControllers/MainOverlayWidgetController.h"

// ~ Core
#include "Core/FunctionLibraries/ObsidianUIFunctionLibrary.h"

// ~ Project
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Obsidian/ObsidianGameplayTags.h"

void UMainOverlayWidgetController::OnWidgetControllerSetupCompleted()
{
	check(ObsidianAbilitySystemComponent);
	check(AttributesComponent);
	check(ObsidianPlayerController);
	
	HandleBindingCallbacks(ObsidianAbilitySystemComponent);
	
	ObsidianPlayerController->OnEnemyActorHoveredDelegate.BindDynamic(this, &ThisClass::UpdateHoveringOverTarget);
	ObsidianPlayerController->OnBossDetectedPlayerDelegate.BindDynamic(this, &ThisClass::UpdateBossDetectionInfo);
	
	ObsidianAbilitySystemComponent->OnEffectAppliedAssetTags.AddUObject(this, &ThisClass::HandleEffectApplied);
	
	SetInitialAttributeValues();

	ObsidianAbilitySystemComponent->OnAuraDisabledDelegate.BindDynamic(this, &ThisClass::DestroyAuraWidget);
}

FObsidianSpecialResourceVisuals UMainOverlayWidgetController::GetSpecialResourceVisuals() const
{
	if(AttributesComponent)
	{
		return	AttributesComponent->GetSpecialResourceVisuals();
	}
	return FObsidianSpecialResourceVisuals();
}

void UMainOverlayWidgetController::HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC)
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

void UMainOverlayWidgetController::SetInitialAttributeValues() const
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

void UMainOverlayWidgetController::SetInitialStaggerMeter() const
{
	OnStaggerMeterChangedDelegate.Broadcast(AttributesComponent->GetStaggerMeter());
	OnMaxStaggerMeterChangedDelegate.Broadcast(AttributesComponent->GetMaxStaggerMeter());
}

void UMainOverlayWidgetController::SetInitialExperienceValues() const
{
	OnExperienceChangedDelegate.Execute(AttributesComponent->GetExperience());
	OnMaxExperienceChangedDelegate.Execute(AttributesComponent->GetMaxExperience(), MaxExperienceOldValue);
}

void UMainOverlayWidgetController::HandleEffectApplied(const FObsidianEffectUIData& UIData)
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

void UMainOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChangedDelegate.Broadcast(Data.NewValue);
}

void UMainOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChangedDelegate.Broadcast(Data.NewValue);
}

void UMainOverlayWidgetController::SpecialResourceChanged(const FOnAttributeChangeData& Data) const
{
	OnSpecialResourceChangedDelegate.Broadcast(Data.NewValue);
}

void UMainOverlayWidgetController::MaxSpecialResourceChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxSpecialResourceChangedDelegate.Broadcast(Data.NewValue);
}

void UMainOverlayWidgetController::ExperienceChanged(const FOnAttributeChangeData& Data) const
{
	if(OnExperienceChangedDelegate.IsBound())
	{
		OnExperienceChangedDelegate.Execute(Data.NewValue);
	}
}

void UMainOverlayWidgetController::MaxExperienceChanged(const FOnAttributeChangeData& Data)
{
	if(OnMaxExperienceChangedDelegate.IsBound())
	{
		MaxExperienceOldValue = Data.OldValue;
		OnMaxExperienceChangedDelegate.Execute(Data.NewValue, MaxExperienceOldValue);
	}
}

void UMainOverlayWidgetController::PassiveSkillPointsChanged(const FOnAttributeChangeData& Data) const
{
	OnPassiveSkillPointsChangedDelegate.Broadcast(Data.NewValue);
}

void UMainOverlayWidgetController::AscensionPointsChanged(const FOnAttributeChangeData& Data) const
{
	OnAscensionPointsChangedDelegate.Broadcast(Data.NewValue);
}

void UMainOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChangedDelegate.Broadcast(Data.NewValue);
}

void UMainOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChangedDelegate.Broadcast(Data.NewValue);
}

void UMainOverlayWidgetController::EnergyShieldChanged(const FOnAttributeChangeData& Data) const
{
	OnEnergyShieldChangedDelegate.Broadcast(Data.NewValue);
}

void UMainOverlayWidgetController::MaxEnergyShieldChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxEnergyShieldChangedDelegate.Broadcast(Data.NewValue);
}

void UMainOverlayWidgetController::StaggerMeterChanged(const FOnAttributeChangeData& Data) const
{
	OnStaggerMeterChangedDelegate.Broadcast(Data.NewValue);
}

void UMainOverlayWidgetController::MaxStaggerMeterChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxStaggerMeterChangedDelegate.Broadcast(Data.NewValue);
}

void UMainOverlayWidgetController::UpdateHoveringOverTarget(AActor* TargetActor, const bool bHoveredOver)
{
	OnUpdateRegularEnemyTargetForHealthBarDelegate.Broadcast(TargetActor, bHoveredOver);
}

void UMainOverlayWidgetController::UpdateBossDetectionInfo(AActor* BossActor, const bool bSeen)
{
	OnUpdateBossEnemyTargetForHealthBarDelegate.Broadcast(BossActor, bSeen);
}

void UMainOverlayWidgetController::DestroyAuraWidget(const FGameplayTag AuraWidgetTag)
{
	if(OnAuraWidgetDestructionInfoReceivedDelegate.IsBound())
	{
		OnAuraWidgetDestructionInfoReceivedDelegate.Execute(AuraWidgetTag);
	}
}

void UMainOverlayWidgetController::UpdateHealthInfoGlobe(const float Magnitude) const
{
	EffectUIHealthGlobeDataDelegate.Broadcast(0, Magnitude);
}

void UMainOverlayWidgetController::UpdateManaInfoGlobe(const float Magnitude) const 
{
	EffectUIManaGlobeDataDelegate.Broadcast(0, Magnitude);
}




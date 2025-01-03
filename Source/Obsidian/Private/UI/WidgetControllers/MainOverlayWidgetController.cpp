// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/WidgetControllers/MainOverlayWidgetController.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Core/ObsidianUIFunctionLibrary.h"
#include "Obsidian/ObsidianGameplayTags.h"

void UMainOverlayWidgetController::OnWidgetControllerSetupCompleted()
{
	UObsidianAbilitySystemComponent* ObsidianASC = Cast<UObsidianAbilitySystemComponent>(AbilitySystemComponent);
	check(ObsidianASC);
	check(AttributesComponent);

	HandleBindingCallbacks(ObsidianASC);

	AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(PlayerController);
	check(ObsidianPC);

	ObsidianPC->OnEnemyActorHoveredDelegate.BindDynamic(this, &ThisClass::UpdateHoveringOverTarget);
	ObsidianPC->OnBossDetectedPlayerDelegate.BindDynamic(this, &ThisClass::UpdateBossDetectionInfo);
	
	ObsidianASC->OnEffectAppliedAssetTags.AddUObject(this, &ThisClass::HandleEffectApplied);
	
	SetInitialAttributeValues();

	ObsidianASC->OnAuraDisabledDelegate.BindDynamic(this, &ThisClass::DestroyAuraWidget);
}

void UMainOverlayWidgetController::HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC)
{
	/** Hero Set */
	ManaChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetManaAttribute()).AddUObject(this, &ThisClass::ManaChanged);
	MaxManaChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxManaAttribute()).AddUObject(this, &ThisClass::MaxManaChanged);
	SpecialResourceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetSpecialResourceAttribute()).AddUObject(this, &ThisClass::SpecialResourceChanged);
	MaxSpecialResourceChangedDelegateHandle = ObsidianASC->GetGameplayAttributeValueChangeDelegate(AttributesComponent->GetMaxSpecialResourceAttribute()).AddUObject(this, &ThisClass::MaxSpecialResourceChanged);

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
}

void UMainOverlayWidgetController::SetInitialStaggerMeter() const
{
	OnStaggerMeterChangedDelegate.Broadcast(AttributesComponent->GetStaggerMeter());
	OnMaxStaggerMeterChangedDelegate.Broadcast(AttributesComponent->GetMaxStaggerMeter());
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
	OnAuraWidgetDestructionInfoReceivedDelegate.ExecuteIfBound(AuraWidgetTag);
}

void UMainOverlayWidgetController::UpdateHealthInfoGlobe(const float Magnitude) const
{
	EffectUIHealthGlobeDataDelegate.Broadcast(0, Magnitude);
}

void UMainOverlayWidgetController::UpdateManaInfoGlobe(const float Magnitude) const 
{
	EffectUIManaGlobeDataDelegate.Broadcast(0, Magnitude);
}




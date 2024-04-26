// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/WidgetControllers/MainOverlayWidgetController.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"

void UMainOverlayWidgetController::BroadcastControllerToAttributesComp(UObsidianAttributesComponent* AC)
{
	UObsidianHeroAttributesComponent* HeroAttributesComp = CastChecked<UObsidianHeroAttributesComponent>(AC);
	HeroAttributesComp->SetMainWidgetController(this);
}

void UMainOverlayWidgetController::OnWidgetControllerSetupCompleted()
{
	UObsidianAbilitySystemComponent* ObsidianASC = Cast<UObsidianAbilitySystemComponent>(AbilitySystemComponent);
	check(ObsidianASC);
	
	ObsidianASC->EffectAppliedAssetTags.AddLambda(
		[this](const FObsidianEffectUIData& UIData)
		{
			for(const FGameplayTag& Tag : UIData.AssetTags)
			{
				const FGameplayTag EffectUIDataTag = FGameplayTag::RequestGameplayTag(FName("UI.EffectData"));
				if(Tag.MatchesTag(EffectUIDataTag))
				{
					const FObsidianEffectUIDataWidgetRow* Row = GetDataTableRowByTag<FObsidianEffectUIDataWidgetRow>(UIEffectDataWidgetTable, Tag);
					EffectUIDataWidgetRowDelegate.Broadcast(*Row, UIData.EffectDuration);
				}

				if(UIData.EffectDurationPolicy == EGameplayEffectDurationType::HasDuration)
				{
					const FGameplayTag HealthGlobeDataTag = FGameplayTag::RequestGameplayTag(FName("UI.GlobeData.HealingHealth"));
					const FGameplayTag ManaGlobeDataTag = FGameplayTag::RequestGameplayTag(FName("UI.GlobeData.ReplenishingMana"));
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
		});
}

void UMainOverlayWidgetController::UpdateHealthInfoGlobe(const float& Magnitude) const
{
	EffectUIHealthGlobeDataDelegate.Broadcast(0, Magnitude);
}

void UMainOverlayWidgetController::UpdateManaInfoGlobe(const float& Magnitude) const 
{
	EffectUIManaGlobeDataDelegate.Broadcast(0, Magnitude);
}


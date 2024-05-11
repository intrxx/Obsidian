// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/WidgetControllers/MainOverlayWidgetController.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"

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
					FObsidianEffectUIDataWidgetRow* Row = GetDataTableRowByTag<FObsidianEffectUIDataWidgetRow>(UIEffectDataWidgetTable, Tag);
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


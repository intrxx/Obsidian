// Copyright 2024 Michał Ogiński


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

	// Might want to do it in Attributes Comp :hmm:
	
	ObsidianASC->EffectAppliedAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for(const FGameplayTag& Tag : AssetTags)
			{
				FGameplayTag EffectUIDataTag = FGameplayTag::RequestGameplayTag(FName("UI.EffectData"));
				if(Tag.MatchesTag(EffectUIDataTag))
				{
					const FObsidianEffectUIDataWidgetRow* Row = GetDataTableRowByTag<FObsidianEffectUIDataWidgetRow>(UIEffectDataWidgetTable, Tag);
					EffectUIDataWidgetRowDelegate.Broadcast(*Row);
				}
			}
		});
}

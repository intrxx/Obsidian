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
		[](const FGameplayTagContainer& AssetTags)
		{
			for(const FGameplayTag& Tag : AssetTags)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
					FString::Printf(TEXT("Tag Name: %s"), *Tag.GetTagName().ToString()));
			}
		});
}

// Copyright 2024 Michał Ogiński


#include "UI/WidgetControllers/MainOverlayWidgetController.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"

void UMainOverlayWidgetController::BroadcastControllerToAttributesComp(UObsidianAttributesComponent* AC)
{
	UObsidianHeroAttributesComponent* HeroAttributesComp = CastChecked<UObsidianHeroAttributesComponent>(AC);
	HeroAttributesComp->SetMainWidgetController(this);
}

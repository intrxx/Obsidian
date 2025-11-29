// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/WidgetControllers/ObCharacterSelectionWidgetController.h"

#include <Kismet/GameplayStatics.h>

#include "Game/ObsidianFrontEndGameMode.h"
#include "Characters/ObsidianCharacterCreationHero.h"
#include "Characters/Player/ObsidianPlayerController.h"


void UObCharacterSelectionWidgetController::OnWidgetControllerSetupCompleted()
{
	check(ObsidianLocalPlayer);
	check(ObsidianPlayerController);
	
	GatherCreationHeroes();
	GatherViewTargets();
}

void UObCharacterSelectionWidgetController::HighlightCharacterWithTag(const EObsidianHeroClass WithClass)
{
	if(AObsidianCharacterCreationHero* CreationHero = GetCreationHeroForTag(WithClass))
	{
		CreationHero->StartHighlight();
		CreationHero->PlayChooseMeAnimMontage();
	}
}

void UObCharacterSelectionWidgetController::ResetHighlightForCharacterWithTag(const EObsidianHeroClass WithClass)
{
	if(AObsidianCharacterCreationHero* CreationHero = GetCreationHeroForTag(WithClass))
	{
		CreationHero->StopHighlight();
		CreationHero->StopPlayingChooseMeAnimMontage();
	}
}

AObsidianCharacterCreationHero* UObCharacterSelectionWidgetController::GetCreationHeroForTag(const EObsidianHeroClass ForClass)
{
	for(AObsidianCharacterCreationHero* Hero : CreationHeroes)
	{
		if(Hero && Hero->HeroClass == ForClass)
		{
			return Hero;
		}
	}
	return nullptr;
}

void UObCharacterSelectionWidgetController::SetupCameraForCreationPanel()
{
	check(ObsidianPlayerController)
	check(CharacterCreationViewTarget)
	ObsidianPlayerController->SetViewTarget(CharacterCreationViewTarget);
}

void UObCharacterSelectionWidgetController::ResetCameraToDefaultViewTarget()
{
	check(ObsidianPlayerController)
	check(MainMenuDefaultViewTarget)
	ObsidianPlayerController->SetViewTarget(MainMenuDefaultViewTarget);
}

void UObCharacterSelectionWidgetController::GatherCreationHeroes()
{
	CreationHeroes.Empty();
	
	TArray<AActor*> CharacterCreationActors; 
	UGameplayStatics::GetAllActorsOfClass(this, AObsidianCharacterCreationHero::StaticClass(),
		CharacterCreationActors);

	for(AActor* Actor : CharacterCreationActors)
	{
		if(AObsidianCharacterCreationHero* CCHero = Cast<AObsidianCharacterCreationHero>(Actor))
		{
			CreationHeroes.Add(CCHero);	
		}
	}
}

void UObCharacterSelectionWidgetController::GatherViewTargets()
{
	check(ObsidianPlayerController)
	if (const UWorld* World = ObsidianPlayerController->GetWorld())
	{
		TArray<AActor*> OutViewTargets;
		UGameplayStatics::GetAllActorsWithTag(World, ObsidianActorTags::ViewTarget, OutViewTargets);

		for (AActor* ViewTarget : OutViewTargets)
		{
			if (ViewTarget->ActorHasTag(ObsidianActorTags::DefaultMainMenuViewTarget))
			{
				MainMenuDefaultViewTarget = ViewTarget;
			}
			else if (ViewTarget->ActorHasTag(ObsidianActorTags::CharacterCreationViewTarget))
			{
				CharacterCreationViewTarget = ViewTarget;
			}
		}
	}
}


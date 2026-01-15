// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/WidgetControllers/ObCharacterSelectionWidgetController.h"

#include <Kismet/GameplayStatics.h>

#include "Game/ObsidianFrontEndGameMode.h"
#include "Characters/ObsidianCharacterCreationHero.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Obsidian/ObsidianGameModule.h"


void UObCharacterSelectionWidgetController::OnWidgetControllerSetupCompleted()
{
	check(OwnerLocalPlayer.IsValid());
	check(OwnerPlayerController.IsValid());
	
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
	check(CharacterCreationViewTarget)
	if (CharacterCreationViewTarget)
	{
		check(OwnerPlayerController.IsValid())
		if (AObsidianPlayerController* PlayerController = OwnerPlayerController.Get())
		{
			PlayerController->SetViewTarget(CharacterCreationViewTarget);
		}
	}
}

void UObCharacterSelectionWidgetController::ResetCameraToDefaultViewTarget()
{
	check(MainMenuDefaultViewTarget)
	if (MainMenuDefaultViewTarget)
	{
		check(OwnerPlayerController.IsValid())
		if (AObsidianPlayerController* PlayerController = OwnerPlayerController.Get())
		{
			PlayerController->SetViewTarget(MainMenuDefaultViewTarget);
		}
	}
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
	check(OwnerPlayerController.IsValid())
	AObsidianPlayerController* PlayerController = OwnerPlayerController.Get();
	if (PlayerController == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("PlayerController is invalid in [%hs]"), __FUNCTION__);
	}
	
	if (const UWorld* World = OwnerPlayerController->GetWorld())
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


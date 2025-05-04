// Copyright 2024 out of sCope team - Michał Ogiński

#include "Game/ObsidianFrontEndGameMode.h"
#include "Characters/ObsidianCharacterCreationHero.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Obsidian/ObsidianGameplayTags.h"

AObsidianFrontEndGameMode::AObsidianFrontEndGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerControllerClass = AObsidianPlayerController::StaticClass();
	
}

void AObsidianFrontEndGameMode::HighlightCharacterWithTag(const FGameplayTag& Tag)
{
	if(AObsidianCharacterCreationHero* CreationHero = GetCreationHeroForTag(Tag))
	{
		CreationHero->StartHighlight();
		CreationHero->PlayChooseMeAnimMontage();
	}
}

void AObsidianFrontEndGameMode::ResetHighlightForCharacterWithTag(const FGameplayTag& Tag)
{
	if(AObsidianCharacterCreationHero* CreationHero = GetCreationHeroForTag(Tag))
	{
		CreationHero->StopHighlight();
		CreationHero->StopPlayingChooseMeAnimMontage();
	}
}

void AObsidianFrontEndGameMode::BeginPlay()
{
	Super::BeginPlay();

	GatherCreationHeroes();
}

AObsidianCharacterCreationHero* AObsidianFrontEndGameMode::GetCreationHeroForTag(const FGameplayTag& Tag)
{
	for(AObsidianCharacterCreationHero* Hero : CreationHeroes)
	{
		if(Hero && Hero->HeroTag == Tag)
		{
			return Hero;
		}
	}
	return nullptr;
}

void AObsidianFrontEndGameMode::GatherCreationHeroes()
{
	TArray<AActor*> CharacterCreationActors; 
	UGameplayStatics::GetAllActorsOfClass(this, AObsidianCharacterCreationHero::StaticClass(), CharacterCreationActors);

	for(AActor* Actor : CharacterCreationActors)
	{
		if(AObsidianCharacterCreationHero* CCHero = Cast<AObsidianCharacterCreationHero>(Actor))
		{
			CreationHeroes.Add(CCHero);	
		}
	}
}

	

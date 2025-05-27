// Copyright 2024 out of sCope team - Michał Ogiński

#include "Game/ObsidianGameMode.h"

// ~ Core

// ~ Project
#include "Characters/Heroes/ObsidianHero.h"
#include "Game/ObsidianGameInstance.h"

void AObsidianGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	if(const UObsidianGameInstance* ObsidianGameInstance = Cast<UObsidianGameInstance>(GetGameInstance()))
	{
		if(const TSubclassOf<AObsidianHero> HeroClass = ObsidianGameInstance->ChosenHero.LoadSynchronous())
		{
			DefaultPawnClass = HeroClass;
		}
	}
	
	Super::InitGame(MapName, Options, ErrorMessage);
}

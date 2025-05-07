// Copyright 2024 out of sCope team - Michał Ogiński


#include "Game/ObsidianGameMode.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "Game/ObsidianGameInstance.h"


void AObsidianGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	UObsidianGameInstance* ObsidianGameInstance = Cast<UObsidianGameInstance>(GetGameInstance());
	if(ObsidianGameInstance)
	{
		DefaultPawnClass = ObsidianGameInstance->ChosenHero.LoadSynchronous();
	}
	
	Super::InitGame(MapName, Options, ErrorMessage);
}

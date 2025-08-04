// Copyright 2024 out of sCope team - Michał Ogiński

#include "Game/ObsidianGameMode.h"

// ~ Core

// ~ Project
#include "Characters/Heroes/ObsidianHero.h"
#include "Game/ObsidianGameInstance.h"
#include "InventoryItems/Items/ItemSpecific/ObsidianTownPortal.h"

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

void AObsidianGameMode::RegisterPortal(AObsidianTownPortal* InNewTownPortal)
{
	if(InNewTownPortal)
	{
		ActiveTownPortals.Add(InNewTownPortal);
	}
}

bool AObsidianGameMode::CanCreatePortal() const
{
	bool bCanCreate = true;
	for(const AObsidianTownPortal* TownPortal : ActiveTownPortals)
	{
		if(TownPortal && IsValid(TownPortal) && TownPortal->IsOpeningInProgress())
		{
			bCanCreate = false;
		}
	}
	return bCanCreate;
}

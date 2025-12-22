// Copyright 2024 out of sCope team - Michał Ogiński

#include <Game/ObsidianGameMode.h>


#include "Characters/Heroes/ObsidianHero.h"
#include "Game/Save/ObsidianHeroSaveGame.h"
#include "Game/Save/ObsidianSaveGameSubsystem.h"
#include "InventoryItems/Items/ItemSpecific/ObsidianTownPortal.h"

void AObsidianGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	if(const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UObsidianSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UObsidianSaveGameSubsystem>())
		{
			if(UObsidianHeroSaveGame* SaveGameObject = SaveGameSubsystem->GetCurrentHeroSaveGameObject())
			{
				FObsidianHeroSaveData HeroSaveData = SaveGameObject->GetHeroSaveData();
				if (const TSubclassOf<AObsidianHero> HeroClass = HeroSaveData.InitializationSaveData.HeroObjectClass.LoadSynchronous())
				{
					DefaultPawnClass = HeroClass;
				}

				if (HeroSaveData.bOnline)
				{
					CurrentNetworkType = EObsidianGameNetworkType::OnlineCoop;
				}
				else
				{
					CurrentNetworkType = EObsidianGameNetworkType::OfflineSolo;
				}
			}
		}
	}
	
	Super::InitGame(MapName, Options, ErrorMessage);
}

EObsidianGameNetworkType AObsidianGameMode::GetCurrentNetworkType() const
{
	return CurrentNetworkType;
}

UObsidianEnemyTypeInfo* AObsidianGameMode::GetEnemyTypeInfo() const
{
	return EnemyTypeInfo;
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

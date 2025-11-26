// Copyright 2024 out of sCope team - Michał Ogiński

#include "Game/Save/ObsidianHeroSaveGame.h"

#include "Game/Save/ObsidianSaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"


void UObsidianHeroSaveGame::InitWithSaveSystem(UObsidianSaveGameSubsystem* InSaveGameSubsystem)
{
	SaveGameSubsystem = InSaveGameSubsystem;
}

void UObsidianHeroSaveGame::InitializeHeroSaveData(const FObsidianHeroInitializationSaveData& InInitializationSaveData)
{
	HeroSaveData.InitializationSaveData = InInitializationSaveData;
}

void UObsidianHeroSaveGame::SetHeroGameplayData(const FObsidianHeroGameplaySaveData& InGameplaySaveData)
{
	HeroSaveData.GameplaySaveData = InGameplaySaveData;
}

FObsidianHeroSaveData UObsidianHeroSaveGame::GetHeroSaveData()
{
	return HeroSaveData;
}

void UObsidianHeroSaveGame::SetSaveID(const uint16 InSaveID)
{
	HeroSaveData.SaveID = InSaveID;
}

uint16 UObsidianHeroSaveGame::GetSaveID() const
{
	return HeroSaveData.SaveID;
}

void UObsidianHeroSaveGame::HandlePostSave(bool bSuccess)
{
	if (SaveGameSubsystem.IsValid())
	{
		SaveGameSubsystem->HandleSavingHeroSaveFinished(bSuccess, this);
	}
	else if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (const UWorld* World = LocalPlayer->GetWorld())
		{
			if (const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(World))
			{
				if (UObsidianSaveGameSubsystem* ObSaveGameSubsystem = GameInstance->GetSubsystem<UObsidianSaveGameSubsystem>())
				{
					SaveGameSubsystem = ObSaveGameSubsystem;
					SaveGameSubsystem->HandleSavingHeroSaveFinished(bSuccess, this);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogObsidianSaveSystem, Error, TEXT("Failed to HandlePostSave on [%s]. "), *GetNameSafe(this));
	}

	Super::HandlePostSave(bSuccess);
}

void UObsidianHeroSaveGame::HandlePostLoad()
{
	if (SaveGameSubsystem.IsValid())
	{
		SaveGameSubsystem->HandleLoadingHeroSaveFinished(this);
	}
	else if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (const UWorld* World = LocalPlayer->GetWorld())
		{
			if (const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(World))
			{
				if (UObsidianSaveGameSubsystem* ObSaveGameSubsystem = GameInstance->GetSubsystem<UObsidianSaveGameSubsystem>())
				{
					SaveGameSubsystem = ObSaveGameSubsystem;
					SaveGameSubsystem->HandleLoadingHeroSaveFinished(this);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogObsidianSaveSystem, Error, TEXT("Failed to HandlePostLoad on [%s]. "), *GetNameSafe(this));
	}

	Super::HandlePostLoad();
}

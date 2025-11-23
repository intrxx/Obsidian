// Copyright 2024 out of sCope team - Michał Ogiński

#include "Game/Save/ObsidianSaveGame.h"

#include "Game/Save/ObsidianSaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"


void UObsidianSaveGame::InitWithSaveSystem(UObsidianSaveGameSubsystem* InSaveGameSubsystem)
{
	SaveGameSubsystem = InSaveGameSubsystem;
}

void UObsidianSaveGame::InitializeHeroSaveData(const FObsidianHeroInitializationSaveData& InInitializationSaveData)
{
	HeroSaveData.InitializationSaveData = InInitializationSaveData;
}

void UObsidianSaveGame::SetHeroGameplayData(const FObsidianHeroGameplaySaveData& InGameplaySaveData)
{
	HeroSaveData.GameplaySaveData = InGameplaySaveData;
}

FObsidianHeroSaveData UObsidianSaveGame::GetHeroSaveData()
{
	return HeroSaveData;
}

void UObsidianSaveGame::HandlePostSave(bool bSuccess)
{
	if (SaveGameSubsystem.IsValid())
	{
		SaveGameSubsystem->HandleSavingFinished(bSuccess, this);
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
					SaveGameSubsystem->HandleSavingFinished(bSuccess, this);
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

void UObsidianSaveGame::HandlePostLoad()
{
	if (SaveGameSubsystem.IsValid())
	{
		SaveGameSubsystem->HandleLoadingFinished(this);
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
					SaveGameSubsystem->HandleLoadingFinished(this);
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

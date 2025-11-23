// Copyright 2024 out of sCope team - Michał Ogiński

#include "Game/Save/ObsidianSaveGameSubsystem.h"


#include "Characters/Player/ObsidianLocalPlayer.h"
#include "Game/Save/ObsidianSaveableInterface.h"
#include "Game/Save/ObsidianSaveGame.h"

DEFINE_LOG_CATEGORY(LogObsidianSaveSystem)

UObsidianSaveGame* UObsidianSaveGameSubsystem::GetSaveGameObject()
{
	return ObsidianSaveGame;
}

void UObsidianSaveGameSubsystem::CreateSaveGameObject(const UObsidianLocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer))
	{
		UE_LOG(LogObsidianSaveSystem, Display, TEXT("Creating Save Object for [%s]. "),
			*GetNameSafe(LocalPlayer));
		
		SaveSlotName = FString("MySlotName");
		if (ULocalPlayerSaveGame* LocalSaveGame = UObsidianSaveGame::CreateNewSaveGameForLocalPlayer(
			UObsidianSaveGame::StaticClass(), LocalPlayer, SaveSlotName))
		{
			ObsidianSaveGame = Cast<UObsidianSaveGame>(LocalSaveGame);
			ObsidianSaveGame->InitWithSaveSystem(this);
			return;
		}
	}

	UE_LOG(LogObsidianSaveSystem, Error, TEXT("Failed to create Save Object for LocalPlayer!"));
}

void UObsidianSaveGameSubsystem::RegisterSaveable(AActor* SaveActor)
{
	check(!SaveableActors.Contains(SaveActor) && SaveActor->GetClass()->ImplementsInterface(
		UObsidianSaveableInterface::StaticClass()))
	SaveableActors.Add(SaveActor);
}

void UObsidianSaveGameSubsystem::UnregisterSaveable(AActor* SaveActor)
{
	check(SaveableActors.Contains(SaveActor))
	SaveableActors.Remove(SaveActor);
}

void UObsidianSaveGameSubsystem::RequestSaveGame(const bool bAsync, const UObsidianLocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer))
	{
		UE_LOG(LogObsidianSaveSystem, Display, TEXT("Requested Save for [%s]. "), *GetNameSafe(LocalPlayer));
		
		for (TWeakObjectPtr<AActor> SaveActor : SaveableActors)
		{
			if (SaveActor.IsValid() == false)
			{
				continue;
			}

			if (IObsidianSaveableInterface* SaveableInterface = Cast<IObsidianSaveableInterface>(SaveActor))
			{
				SaveableInterface->SaveData(ObsidianSaveGame);
			}
		}
	
		if (bAsync)
		{
			SaveGameForPlayerAsync(LocalPlayer);
			return;
		}
		SaveGameForPlayer(LocalPlayer);
	}

	UE_LOG(LogObsidianSaveSystem, Error, TEXT("Failed to Save for invalid LocalPlayer!"));
	OnSavingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::RequestSaveInitialHeroSave(const bool bAsync,
	const FObsidianHeroInitializationSaveData& HeroInitializationSaveData, const UObsidianLocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer))
	{
		UE_LOG(LogObsidianSaveSystem, Display, TEXT("Requested Initial Hero Save for [%s]. "),
			*GetNameSafe(LocalPlayer));
		
		if (ObsidianSaveGame)
		{
			ObsidianSaveGame->InitializeHeroSaveData(HeroInitializationSaveData);
		}
	
		if (bAsync)
		{
			SaveGameForPlayerAsync(LocalPlayer);
		}
		else
		{
			SaveGameForPlayer(LocalPlayer);
		}
	}
	else
	{
#if !UE_BUILD_SHIPPING
	const bool bValidLocalPlayer = LocalPlayer == nullptr;
	const bool bValidSaveGame = ObsidianSaveGame == nullptr;
	const FString ValidityLocalPlayer = bValidLocalPlayer ? FString("[Local Player Invalid]") : FString("");
	const FString ValiditySaveGame = bValidSaveGame ? FString(", [Save Game Object Invalid]") : FString("");
	UE_LOG(LogObsidianSaveSystem, Error, TEXT("Failed to perform Initial Hero Save, invalid: %s %s"),
		*ValidityLocalPlayer, *ValiditySaveGame);
#endif
	OnSavingFinishedDelegate.Broadcast(nullptr, false);
	}
}

void UObsidianSaveGameSubsystem::RequestLoadGame(const bool bAsync, const UObsidianLocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer))
	{
		UE_LOG(LogObsidianSaveSystem, Display, TEXT("Requested Load Game for [%s]. "),
			*GetNameSafe(LocalPlayer));
		
		if (bAsync)
		{
			LoadGameForPlayerAsync(LocalPlayer);
		}
		else
		{
			LoadGameForPlayer(LocalPlayer);
		}
		return;
	}

	UE_LOG(LogObsidianSaveSystem, Error, TEXT("Failed to Load for invalid LocalPlayer!"));
	OnLoadingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::RequestLoadDataForObject(AActor* LoadActor)
{
	if (IObsidianSaveableInterface* SaveableInterface = Cast<IObsidianSaveableInterface>(LoadActor))
	{
		SaveableInterface->LoadData(ObsidianSaveGame);
	}
}

void UObsidianSaveGameSubsystem::SaveGameForPlayer(const UObsidianLocalPlayer* LocalPlayer)
{
	if (ObsidianSaveGame)
	{
		ObsidianSaveGame->SaveGameToSlotForLocalPlayer();
		return;
	}
	OnSavingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::SaveGameForPlayerAsync(const UObsidianLocalPlayer* LocalPlayer)
{
	if (ObsidianSaveGame)
	{
		ObsidianSaveGame->AsyncSaveGameToSlotForLocalPlayer();
		return;
	}
	OnSavingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::LoadGameForPlayer(const UObsidianLocalPlayer* LocalPlayer)
{
	UObsidianSaveGame::LoadOrCreateSaveGameForLocalPlayer(UObsidianSaveGame::StaticClass(), LocalPlayer, SaveSlotName);
}

void UObsidianSaveGameSubsystem::LoadGameForPlayerAsync(const UObsidianLocalPlayer* LocalPlayer)
{
	UObsidianSaveGame::AsyncLoadOrCreateSaveGameForLocalPlayer(UObsidianSaveGame::StaticClass(), LocalPlayer, SaveSlotName,
		FOnLocalPlayerSaveGameLoadedNative::CreateLambda([this](ULocalPlayerSaveGame* SaveGame)
			{
				HandleLoadingFinished(Cast<UObsidianSaveGame>(SaveGame));
			}));
}

void UObsidianSaveGameSubsystem::HandleLoadingFinished(UObsidianSaveGame* SaveGame)
{
	if (SaveGame)
	{
		OnLoadingFinishedDelegate.Broadcast(SaveGame, true);
		return;
	}
	OnLoadingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::HandleSavingFinished(const bool bSuccess, UObsidianSaveGame* SaveGame)
{
	if (bSuccess && SaveGame)
	{
		OnSavingFinishedDelegate.Broadcast(SaveGame, true);
		return;
	}
	OnSavingFinishedDelegate.Broadcast(nullptr, false);
}

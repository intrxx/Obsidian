// Copyright 2024 out of sCope team - Michał Ogiński

#include "Game/Save/ObsidianSaveGameSubsystem.h"


#include "Characters/Player/ObsidianLocalPlayer.h"
#include "Game/Save/ObsidianSaveableInterface.h"
#include "Game/Save/ObsidianHeroSaveGame.h"
#include "Game/Save/ObsidianMasterSaveGame.h"

DEFINE_LOG_CATEGORY(LogObsidianSaveSystem)

UObsidianHeroSaveGame* UObsidianSaveGameSubsystem::GetCurrentHeroSaveGameObject()
{
	return CurrentHeroSaveGame;
}

bool UObsidianSaveGameSubsystem::FillSaveInfosFromMasterSave(const bool bOnline, const UObsidianLocalPlayer* LocalPlayer,
	TArray<FObsidianHeroSaveInfo>& OutHeroInfos)
{
	if (ensure(LocalPlayer))
	{
		if (ObsidianMasterSaveGame == nullptr)
		{
			UE_LOG(LogObsidianSaveSystem, Error, TEXT("ObsidianMasterSaveGame isn't loaded yet in [%hs],"
											 " need to load or create it now!"), __FUNCTION__);
			LoadOrCreateMasterSaveObject(LocalPlayer);
		}

		OutHeroInfos.Append(ObsidianMasterSaveGame->GetHeroSaveInfos(bOnline));
		return !OutHeroInfos.IsEmpty();
	}
	return false;
}

void UObsidianSaveGameSubsystem::LoadOrCreateMasterSaveObject(const UObsidianLocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer))
	{
		UE_LOG(LogObsidianSaveSystem, Display, TEXT("Creating or loading existing Master Save Object for [%s]. "),
			*GetNameSafe(LocalPlayer));
		
		ULocalPlayerSaveGame* SaveGame = UObsidianHeroSaveGame::LoadOrCreateSaveGameForLocalPlayer(
			UObsidianMasterSaveGame::StaticClass(),
			LocalPlayer,
			ObsidianMasterSaveStatics::MasterSaveName);

		check(SaveGame);
		ObsidianMasterSaveGame = Cast<UObsidianMasterSaveGame>(SaveGame);
		return;
	}
	
	UE_LOG(LogObsidianSaveSystem, Error, TEXT("Failed to create or load Master Save Object for LocalPlayer!"));
}

UObsidianHeroSaveGame* UObsidianSaveGameSubsystem::CreateHeroSaveGameObject(const UObsidianLocalPlayer* LocalPlayer,
	const FString& SlotName)
{
	if (ensure(LocalPlayer))
	{
		UE_LOG(LogObsidianSaveSystem, Display, TEXT("Creating Save Object for [%s]. "),
			*GetNameSafe(LocalPlayer));
		
		if (ULocalPlayerSaveGame* LocalSaveGame = UObsidianHeroSaveGame::CreateNewSaveGameForLocalPlayer(
			UObsidianHeroSaveGame::StaticClass(), LocalPlayer, SlotName))
		{
			UObsidianHeroSaveGame* ObsidianHeroSaveGame = Cast<UObsidianHeroSaveGame>(LocalSaveGame);
			ObsidianHeroSaveGame->InitWithSaveSystem(this);
			ObsidianHeroSaveGames.Add(ObsidianHeroSaveGame);
			return ObsidianHeroSaveGame;
		}
	}

	UE_LOG(LogObsidianSaveSystem, Error, TEXT("Failed to create Save Object for LocalPlayer!"));
	return nullptr;
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
	if (ensure(LocalPlayer) && ensure(CurrentHeroSaveGame))
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
				SaveableInterface->SaveData(CurrentHeroSaveGame);
			}
		}
	
		if (bAsync)
		{
			SaveHeroGameForPlayerAsync();
			return;
		}
		SaveHeroGameForPlayer();
	}

	UE_LOG(LogObsidianSaveSystem, Error, TEXT("Failed to Save for invalid LocalPlayer!"));
	OnSavingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::RequestSaveInitialHeroSave(const bool bAsync, const bool bOnline,
	const FObsidianHeroInitializationSaveData& HeroInitializationSaveData, const UObsidianLocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer))
	{
		UE_LOG(LogObsidianSaveSystem, Display, TEXT("Requested Initial Hero Save for [%s]. "),
			*GetNameSafe(LocalPlayer));
		
		check(ObsidianMasterSaveGame);
		const FObsidianAddHeroSaveResult Result = ObsidianMasterSaveGame->AddHero(bOnline, HeroInitializationSaveData);
		UObsidianHeroSaveGame* NewHeroSaveGame = CreateHeroSaveGameObject(LocalPlayer, Result.SaveName);
		NewHeroSaveGame->InitializeHeroSaveData(HeroInitializationSaveData);
		CurrentHeroSaveGame = NewHeroSaveGame;

		//TODO(intrxx) Make it async 
		ObsidianMasterSaveGame->SaveGameToSlotForLocalPlayer();
		
		if (bAsync)
		{
			SaveHeroGameForPlayerAsync();
		}
		else
		{
			SaveHeroGameForPlayer();
		}
		
		return;
	}
	
	UE_LOG(LogObsidianSaveSystem, Error, TEXT("Failed to perform Initial Hero Save for invalid Local Player. "));
	OnSavingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::RequestLoadGame(const bool bAsync, const UObsidianLocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer) && CurrentHeroSaveGame)
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
#if !UE_BUILD_SHIPPING
	else
	{
		const FString LocalPlayerValid = LocalPlayer == nullptr ? TEXT("LocalPlayer") : FString();
		const FString CurrentHeroSaveGameValid = CurrentHeroSaveGame == nullptr ? TEXT("Current Hero Save Game") : FString();
		UE_LOG(LogObsidianSaveSystem, Error, TEXT("Failed to Load for invalid %s %s!"), *LocalPlayerValid,
			*CurrentHeroSaveGameValid);
	}
#endif
	OnLoadingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::RequestLoadDataForObject(AActor* LoadActor)
{
	if (IObsidianSaveableInterface* SaveableInterface = Cast<IObsidianSaveableInterface>(LoadActor))
	{
		SaveableInterface->LoadData(CurrentHeroSaveGame);
	}
}

void UObsidianSaveGameSubsystem::SaveHeroGameForPlayer()
{
	if (CurrentHeroSaveGame)
	{
		CurrentHeroSaveGame->SaveGameToSlotForLocalPlayer();
		return;
	}
	OnSavingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::SaveHeroGameForPlayerAsync()
{
	if (CurrentHeroSaveGame)
	{
		CurrentHeroSaveGame->AsyncSaveGameToSlotForLocalPlayer();
		return;
	}
	OnSavingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::LoadGameForPlayer(const UObsidianLocalPlayer* LocalPlayer)
{
	UObsidianHeroSaveGame::LoadOrCreateSaveGameForLocalPlayer(UObsidianHeroSaveGame::StaticClass(), LocalPlayer,
		CurrentHeroSaveGame->GetSaveSlotName());
}

void UObsidianSaveGameSubsystem::LoadGameForPlayerAsync(const UObsidianLocalPlayer* LocalPlayer)
{
	UObsidianHeroSaveGame::AsyncLoadOrCreateSaveGameForLocalPlayer(UObsidianHeroSaveGame::StaticClass(), LocalPlayer,
		CurrentHeroSaveGame->GetSaveSlotName(),
		FOnLocalPlayerSaveGameLoadedNative::CreateLambda([this](ULocalPlayerSaveGame* SaveGame)
			{
				HandleLoadingFinished(Cast<UObsidianHeroSaveGame>(SaveGame));
			}));
}

void UObsidianSaveGameSubsystem::HandleLoadingFinished(UObsidianHeroSaveGame* SaveGame)
{
	if (SaveGame)
	{
		OnLoadingFinishedDelegate.Broadcast(SaveGame, true);
		return;
	}
	OnLoadingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::HandleSavingFinished(const bool bSuccess, UObsidianHeroSaveGame* SaveGame)
{
	if (bSuccess && SaveGame)
	{
		OnSavingFinishedDelegate.Broadcast(SaveGame, true);
		return;
	}
	OnSavingFinishedDelegate.Broadcast(nullptr, false);
}

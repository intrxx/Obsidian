// Copyright 2024 out of sCope team - Michał Ogiński

#include "Game/Save/ObsidianSaveGameSubsystem.h"


#include "Characters/Player/ObsidianLocalPlayer.h"
#include "Game/Save/ObsidianSaveableInterface.h"
#include "Game/Save/ObsidianHeroSaveGame.h"
#include "Game/Save/ObsidianMasterSaveGame.h"
#include "Kismet/GameplayStatics.h"

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
			UObsidianMasterSaveGame::StaticClass(), LocalPlayer, ObsidianMasterSaveStatics::MasterSaveName);

		check(SaveGame);
		ObsidianMasterSaveGame = Cast<UObsidianMasterSaveGame>(SaveGame);
		return;
	}
	
	UE_LOG(LogObsidianSaveSystem, Error, TEXT("Failed to create or load Master Save Object for LocalPlayer!"));
}

UObsidianHeroSaveGame* UObsidianSaveGameSubsystem::CreateHeroSaveGameObject(const UObsidianLocalPlayer* LocalPlayer,
	const FString& SlotName, const uint16 SaveID)
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
			ObsidianHeroSaveGame->SetSaveID(SaveID);
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

		check(ObsidianMasterSaveGame)
		check(CurrentHeroSaveGame)
		const bool bUpdateSuccess = ObsidianMasterSaveGame->UpdateHeroSave(CurrentHeroSaveGame->GetSaveID(),
			CurrentHeroSaveGame->IsOnline(), CurrentHeroSaveGame->GetHeroLevel());
		if (bUpdateSuccess)
		{
			ObsidianMasterSaveGame->AsyncSaveGameToSlotForLocalPlayer();
		}
		else
		{
			UE_LOG(LogObsidianSaveSystem, Error, TEXT("Failed to update Save Info in Master Save."));
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
		UObsidianHeroSaveGame* NewHeroSaveGame = CreateHeroSaveGameObject(LocalPlayer, Result.SaveName, Result.SaveID);
		NewHeroSaveGame->InitializeHeroSaveData(HeroInitializationSaveData, bOnline);
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

void UObsidianSaveGameSubsystem::RequestLoadHeroSaveGameWithID(const bool bAsync, const uint16 SaveID, const bool bOnline,
	const UObsidianLocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer))
	{
		check(ObsidianMasterSaveGame)
		const FString HeroSaveName = ObsidianMasterSaveGame->GetSaveNameForID(SaveID, bOnline);
		RequestLoadGame(bAsync, HeroSaveName, LocalPlayer);
	}
}

void UObsidianSaveGameSubsystem::RequestLoadGame(const bool bAsync, const FString& SlotName,
	const UObsidianLocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer))
	{
		UE_LOG(LogObsidianSaveSystem, Display, TEXT("Requested Load Game for [%s]. "),
			*GetNameSafe(LocalPlayer));
		
		if (bAsync)
		{
			LoadGameForPlayerAsync(SlotName, LocalPlayer);
		}
		else
		{
			LoadGameForPlayer(SlotName, LocalPlayer);
		}
		return;
	}

	UE_LOG(LogObsidianSaveSystem, Error, TEXT("Failed to perform load Hero Save with invalid Local Player. "));
	OnLoadingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::RequestLoadDataForObject(AActor* LoadActor)
{
	if (IObsidianSaveableInterface* SaveableInterface = Cast<IObsidianSaveableInterface>(LoadActor))
	{
		SaveableInterface->LoadData(CurrentHeroSaveGame);
	}
}

bool UObsidianSaveGameSubsystem::DeleteHeroSave(const uint16 SaveID, const bool bOnline)
{
	check(ObsidianMasterSaveGame)
	const FString SlotNameToDelete = ObsidianMasterSaveGame->GetSaveNameForID(SaveID, bOnline);
	const bool bSuccess = UGameplayStatics::DeleteGameInSlot(SlotNameToDelete, 0);
	if (bSuccess)
	{
		if (!ObsidianMasterSaveGame->DeleteHero(SaveID, bOnline))
		{
			UE_LOG(LogObsidianSaveSystem, Error, TEXT("[%s] save with id [%d], of retrieved name [%s],"
				" could not be deleted on Master Save Object."), bOnline ? TEXT("Online") : TEXT("Offline"), SaveID,
				*SlotNameToDelete)
		}
	}
	else
	{
		if (!UGameplayStatics::DoesSaveGameExist(SlotNameToDelete, 0))
		{
			UE_LOG(LogObsidianSaveSystem, Error, TEXT("[%s] save with id [%d], of retrieved name [%s] does not exist,"
				" and could not be deleted."), bOnline ? TEXT("Online") : TEXT("Offline"), SaveID, *SlotNameToDelete)
		}
		else
		{
			UE_LOG(LogObsidianSaveSystem, Error, TEXT("[%s] save with id [%d], of retrieved name [%s], could not be deleted."),
				bOnline ? TEXT("Online") : TEXT("Offline"), SaveID, *SlotNameToDelete)
		}
	}
	return bSuccess;
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

void UObsidianSaveGameSubsystem::LoadGameForPlayer(const FString& SlotName, const UObsidianLocalPlayer* LocalPlayer)
{
	UObsidianHeroSaveGame::LoadOrCreateSaveGameForLocalPlayer(UObsidianHeroSaveGame::StaticClass(), LocalPlayer,
		SlotName);
}

void UObsidianSaveGameSubsystem::LoadGameForPlayerAsync(const FString& SlotName, const UObsidianLocalPlayer* LocalPlayer)
{
	UObsidianHeroSaveGame::AsyncLoadOrCreateSaveGameForLocalPlayer(UObsidianHeroSaveGame::StaticClass(), LocalPlayer,
		SlotName,FOnLocalPlayerSaveGameLoadedNative::CreateLambda([this](ULocalPlayerSaveGame* SaveGame)
			{
				HandleLoadingHeroSaveFinished(Cast<UObsidianHeroSaveGame>(SaveGame));
			}));
}

void UObsidianSaveGameSubsystem::HandleLoadingHeroSaveFinished(UObsidianHeroSaveGame* SaveGame)
{
	if (SaveGame)
	{
		CurrentHeroSaveGame = SaveGame;
		OnLoadingFinishedDelegate.Broadcast(SaveGame, true);
		return;
	}
	OnLoadingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::HandleSavingHeroSaveFinished(const bool bSuccess, UObsidianHeroSaveGame* SaveGame)
{
	if (bSuccess && SaveGame)
	{
		OnSavingFinishedDelegate.Broadcast(SaveGame, true);
		return;
	}
	OnSavingFinishedDelegate.Broadcast(nullptr, false);
}

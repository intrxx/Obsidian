// Copyright 2026 out of sCope team - intrxx

#include "Game/Save/ObsidianSaveGameSubsystem.h"

#include <Kismet/GameplayStatics.h>

#include "Characters/Player/ObsidianLocalPlayer.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Core/ObsidianGameplayStatics.h"
#include "Game/Save/ObsidianSaveableInterface.h"
#include "Game/Save/ObsidianHeroSaveGame.h"
#include "Game/Save/ObsidianMasterSaveGame.h"
#include "Game/Save/ObsidianSharedStashSaveGame.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "Obsidian/ObsidianLogCategories.h"

UObsidianHeroSaveGame* UObsidianSaveGameSubsystem::GetCurrentHeroSaveGameObject()
{
	return CurrentHeroSaveGame;
}

UObsidianSharedStashSaveGame* UObsidianSaveGameSubsystem::GetStashSaveGameObject(const EObsidianGameNetworkType NetworkType)
{
	ensure(NetworkType != EObsidianGameNetworkType::None);

	if (UObsidianGameplayStatics::IsOfflineNetworkType(NetworkType))
	{
		return OfflineSharedStashData;
	}
	return OnlineSharedStashData;
}

bool UObsidianSaveGameSubsystem::FillSaveInfosFromMasterSave(const bool bOnline, const UObsidianLocalPlayer* LocalPlayer,
                                                             TArray<FObsidianHeroSaveInfo>& OutHeroInfos)
{
	if (ensure(LocalPlayer))
	{
		if (ObsidianMasterSaveGame == nullptr)
		{
			UE_LOG(ObLogSaveSystem, Error, TEXT("ObsidianMasterSaveGame isn't loaded yet in [%hs],"
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
		UE_LOG(ObLogSaveSystem, Log, TEXT("Creating or loading existing Master Save Object for [%s]. "),
			*GetNameSafe(LocalPlayer));
		
		ULocalPlayerSaveGame* SaveGame = UObsidianMasterSaveGame::LoadOrCreateSaveGameForLocalPlayer(
			UObsidianMasterSaveGame::StaticClass(), LocalPlayer, ObsidianSaveStatics::MasterSaveName);

		check(SaveGame);
		ObsidianMasterSaveGame = Cast<UObsidianMasterSaveGame>(SaveGame);
		return;
	}
	
	UE_LOG(ObLogSaveSystem, Error, TEXT("Failed to create or load Master Save Object for LocalPlayer!"));
}

void UObsidianSaveGameSubsystem::AsyncLoadOrCreateSharedStashDataSaveObject(const UObsidianLocalPlayer* LocalPlayer,
	const bool bOnline)
{
	if (ensure(LocalPlayer))
	{
		UE_LOG(ObLogSaveSystem, Log, TEXT("Creating or loading existing [%s] Shared Stash Data for [%s]."),
			bOnline ? TEXT("Online") : TEXT("Offline"), *GetNameSafe(LocalPlayer));

		bool bSuccess = false;
		if (bOnline)
		{
			UObsidianSharedStashSaveGame::AsyncLoadOrCreateSaveGameForLocalPlayer(
				UObsidianSharedStashSaveGame::StaticClass(), LocalPlayer, ObsidianSaveStatics::OnlineStashDataSaveName,
				FOnLocalPlayerSaveGameLoadedNative::CreateWeakLambda(this, [this](ULocalPlayerSaveGame* SaveGame)
					{
						check(SaveGame);
						OnlineSharedStashData = Cast<UObsidianSharedStashSaveGame>(SaveGame);
						OnSharedStashDataLoadedDelegate.Broadcast(OnlineSharedStashData);
					}));
		}
		else
		{
			UObsidianSharedStashSaveGame::AsyncLoadOrCreateSaveGameForLocalPlayer(
				UObsidianSharedStashSaveGame::StaticClass(), LocalPlayer, ObsidianSaveStatics::OfflineStashDataSaveName,
				FOnLocalPlayerSaveGameLoadedNative::CreateWeakLambda(this, [this](ULocalPlayerSaveGame* SaveGame)
					{
						check(SaveGame);
						OfflineSharedStashData = Cast<UObsidianSharedStashSaveGame>(SaveGame);
						OnSharedStashDataLoadedDelegate.Broadcast(OfflineSharedStashData);
					}));
		}
		return;
	}
	
	UE_LOG(ObLogSaveSystem, Error, TEXT("Failed to create or load Master Save Object for LocalPlayer!"));
}

void UObsidianSaveGameSubsystem::RegisterSaveable(AActor* SaveActor)
{
	if (ensure(SaveActor && SaveActor->GetClass()->ImplementsInterface(UObsidianSaveableInterface::StaticClass())))
	{
		SaveableActors.AddUnique(SaveActor);
	}
}

void UObsidianSaveGameSubsystem::UnregisterSaveable(AActor* SaveActor)
{
	SaveableActors.RemoveSingleSwap(SaveActor);
}

void UObsidianSaveGameSubsystem::RequestSaveGame(const UObsidianLocalPlayer* LocalPlayer, const bool bAsync)
{
	if (ensure(LocalPlayer) && ensure(CurrentHeroSaveGame) && ensure(ObsidianMasterSaveGame))
	{
		UE_LOG(ObLogSaveSystem, Log, TEXT("Requested Save for [%s]. "), *GetNameSafe(LocalPlayer));
		
		for (TWeakObjectPtr<AActor> SaveActor : SaveableActors)
		{
			if (SaveActor.IsValid() == false)
			{
				continue;
			}

			// The Hero Save belongs to the Local Player, Pawns of other Players must not write into it.
			if (const APawn* SavePawn = Cast<APawn>(SaveActor.Get()))
			{
				if (SavePawn->IsLocallyControlled() == false)
				{
					continue;
				}
			}

			if (IObsidianSaveableInterface* SaveableInterface = Cast<IObsidianSaveableInterface>(SaveActor))
			{
				SaveableInterface->SaveData(CurrentHeroSaveGame);
			}
		}
		
		const bool bUpdateSuccess = ObsidianMasterSaveGame->UpdateHeroSave(CurrentHeroSaveGame->GetSaveID(),
			CurrentHeroSaveGame->IsOnline(), CurrentHeroSaveGame->GetHeroLevel());
		if (bUpdateSuccess)
		{
			ObsidianMasterSaveGame->AsyncSaveGameToSlotForLocalPlayer();
		}
		else
		{
			UE_LOG(ObLogSaveSystem, Error, TEXT("Failed to update Save Info in Master Save."));
		}
		
			
		if (bAsync)
		{
			SaveHeroGameForPlayerAsync();
			return;
		}
		SaveHeroGameForPlayer();
		return;
	}

	UE_LOG(ObLogSaveSystem, Error, TEXT("Failed to Save for invalid LocalPlayer, CurrentHeroSaveGame or "
		"ObsidianMasterSaveGame!"));
	OnSavingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::RequestSaveInitialHeroSave(const UObsidianLocalPlayer* LocalPlayer, const bool bAsync,
	const bool bOnline, const FObsidianHeroInitializationSaveData& HeroInitializationSaveData)
{
	if (ensure(LocalPlayer) && ensure(ObsidianMasterSaveGame))
	{
		UE_LOG(ObLogSaveSystem, Log, TEXT("Requested Initial Hero Save for [%s]. "),
			*GetNameSafe(LocalPlayer));
		
		const FObsidianAddHeroSaveResult Result = ObsidianMasterSaveGame->AddHero(bOnline, HeroInitializationSaveData);
		UObsidianHeroSaveGame* NewHeroSaveGame = CreateHeroSaveGameObject(LocalPlayer, Result.SaveName, Result.SaveID);
		NewHeroSaveGame->InitializeHeroSaveData(bOnline, HeroInitializationSaveData);
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
	
	UE_LOG(ObLogSaveSystem, Error, TEXT("Failed to perform Initial Hero Save for invalid Local Player."));
	OnSavingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::AsyncSaveSharedStashData(const AObsidianPlayerController* PlayerController,
	const EObsidianGameNetworkType NetworkType)
{
	if (PlayerController == nullptr)
	{
		UE_LOG(ObLogSaveSystem, Error, TEXT("Provided PlayerController is invalid in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = PlayerController->GetPlayerStashComponent();
	if (PlayerStashComponent == nullptr)
	{
		UE_LOG(ObLogSaveSystem, Error, TEXT("PlayerStashComponent is invalid in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianSharedStashSaveGame* SharedStashSaveGame = UObsidianGameplayStatics::IsOfflineNetworkType(NetworkType)
		? OfflineSharedStashData
		: OnlineSharedStashData;
	if (ensure(SharedStashSaveGame))
	{
		const TArray<UObsidianInventoryItemInstance*> SharedStashedItems = PlayerStashComponent->GetAllSharedItems();
		TArray<FObsidianSavedItem>& StashedSavedItems = SharedStashSaveGame->SharedStashData.StashedSavedItems;
		StashedSavedItems.Empty(SharedStashedItems.Num());

		for (UObsidianInventoryItemInstance* Instance : SharedStashedItems)
		{
			if (Instance)
			{
				Instance->ConstructSaveItem(StashedSavedItems.AddDefaulted_GetRef());
			}
		}

		//TODO(intrxx) Save Stash Tabs Cosmetics

		SharedStashSaveGame->AsyncSaveGameToSlotForLocalPlayer();
	}
}

UObsidianHeroSaveGame* UObsidianSaveGameSubsystem::CreateHeroSaveGameObject(const UObsidianLocalPlayer* LocalPlayer,
                                                                            const FString& SlotName, const uint16 SaveID)
{
	if (ensure(LocalPlayer))
	{
		UE_LOG(ObLogSaveSystem, Log, TEXT("Creating Save Object for [%s]. "),
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

	UE_LOG(ObLogSaveSystem, Error, TEXT("Failed to create Save Object for LocalPlayer!"));
	return nullptr;
}

void UObsidianSaveGameSubsystem::RequestLoadHeroSaveGameWithID(const UObsidianLocalPlayer* LocalPlayer, const bool bAsync,
	const uint16 SaveID, const bool bOnline)
{
	if (ensure(LocalPlayer))
	{
		check(ObsidianMasterSaveGame)
		const FString HeroSaveName = ObsidianMasterSaveGame->GetSaveNameForID(SaveID, bOnline);
		RequestLoadGame(LocalPlayer, bAsync, HeroSaveName);
	}
}

void UObsidianSaveGameSubsystem::RequestLoadGame(const UObsidianLocalPlayer* LocalPlayer, const bool bAsync,
	const FString& SlotName)
{
	if (ensure(LocalPlayer))
	{
		UE_LOG(ObLogSaveSystem, Log, TEXT("Requested Load Game for [%s]. "),
			*GetNameSafe(LocalPlayer));
		
		if (bAsync)
		{
			LoadGameForPlayerAsync(LocalPlayer, SlotName);
		}
		else
		{
			LoadGameForPlayer(LocalPlayer, SlotName);
		}
		return;
	}

	UE_LOG(ObLogSaveSystem, Error, TEXT("Failed to perform load Hero Save with invalid Local Player. "));
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
	const int32 UserIndex = ObsidianMasterSaveGame->GetPlatformUserIndex();
	const bool bSuccess = UGameplayStatics::DeleteGameInSlot(SlotNameToDelete, UserIndex);
	if (bSuccess)
	{
		if (ObsidianMasterSaveGame->DeleteHero(SaveID, bOnline))
		{
			ObsidianMasterSaveGame->AsyncSaveGameToSlotForLocalPlayer();
		}
		else
		{
			UE_LOG(ObLogSaveSystem, Error, TEXT("[%s] save with id [%d], of retrieved name [%s],"
				" could not be deleted on Master Save Object."), bOnline ? TEXT("Online") : TEXT("Offline"), SaveID,
				*SlotNameToDelete)
		}

		if (CurrentHeroSaveGame && CurrentHeroSaveGame->GetSaveID() == SaveID && CurrentHeroSaveGame->IsOnline() == bOnline)
		{
			CurrentHeroSaveGame = nullptr;
		}
	}
	else
	{
		if (!UGameplayStatics::DoesSaveGameExist(SlotNameToDelete, UserIndex))
		{
			UE_LOG(ObLogSaveSystem, Error, TEXT("[%s] save with id [%d], of retrieved name [%s] does not exist,"
				" and could not be deleted."), bOnline ? TEXT("Online") : TEXT("Offline"), SaveID, *SlotNameToDelete)
		}
		else
		{
			UE_LOG(ObLogSaveSystem, Error, TEXT("[%s] save with id [%d], of retrieved name [%s], could not be deleted."),
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

void UObsidianSaveGameSubsystem::LoadGameForPlayer(const UObsidianLocalPlayer* LocalPlayer, const FString& SlotName)
{
	UObsidianHeroSaveGame::LoadOrCreateSaveGameForLocalPlayer(UObsidianHeroSaveGame::StaticClass(), LocalPlayer,
		SlotName);
}

void UObsidianSaveGameSubsystem::LoadGameForPlayerAsync(const UObsidianLocalPlayer* LocalPlayer, const FString& SlotName)
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

// Copyright 2026 out of sCope team - intrxx

#include "Game/Save/ObsidianSaveGameSubsystem.h"

#include "Kismet/GameplayStatics.h"

#include "Characters/Player/ObsidianLocalPlayer.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Core/ObsidianGameplayStatics.h"
#include "Game/Save/ObsidianHeroSaveGame.h"
#include "Game/Save/ObsidianMasterSaveGame.h"
#include "Game/Save/ObsidianSaveableInterface.h"
#include "Game/Save/ObsidianSharedStashSaveGame.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "Obsidian/ObsidianLogCategories.h"


UObsidianHeroSaveGame* UObsidianSaveGameSubsystem::GetCurrentHeroSaveGameObject()
{
	return CurrentHeroSaveGame;
}

UObsidianSharedStashSaveGame* UObsidianSaveGameSubsystem::GetStashSaveGameObject(const EObsidianGameNetworkType InNetworkType)
{
	ensure(InNetworkType != EObsidianGameNetworkType::None);

	if (UObsidianGameplayStatics::IsOfflineNetworkType(InNetworkType))
	{
		return OfflineSharedStashData;
	}
	return OnlineSharedStashData;
}

bool UObsidianSaveGameSubsystem::FillSaveInfosFromMasterSave(const bool bInOnline, const UObsidianLocalPlayer* InLocalPlayer,
                                                             TArray<FObsidianHeroSaveInfo>& OutHeroInfos)
{
	if (ensure(InLocalPlayer))
	{
		if (ObsidianMasterSaveGame == nullptr)
		{
			UE_LOG(ObLogSaveSystem, Error, TEXT("ObsidianMasterSaveGame isn't loaded yet in [%hs],"
											 " need to load or create it now!"), __FUNCTION__);
			LoadOrCreateMasterSaveObject(InLocalPlayer);
		}

		OutHeroInfos.Append(ObsidianMasterSaveGame->GetHeroSaveInfos(bInOnline));
		return !OutHeroInfos.IsEmpty();
	}
	return false;
}

void UObsidianSaveGameSubsystem::LoadOrCreateMasterSaveObject(const UObsidianLocalPlayer* InLocalPlayer)
{
	if (ensure(InLocalPlayer))
	{
		UE_LOG(ObLogSaveSystem, Log, TEXT("Creating or loading existing Master Save Object for [%s]. "),
			*GetNameSafe(InLocalPlayer));
		
		ULocalPlayerSaveGame* SaveGame = UObsidianMasterSaveGame::LoadOrCreateSaveGameForLocalPlayer(
			UObsidianMasterSaveGame::StaticClass(), InLocalPlayer, ObsidianSaveStatics::MasterSaveName);

		check(SaveGame);
		ObsidianMasterSaveGame = Cast<UObsidianMasterSaveGame>(SaveGame);
		return;
	}
	
	UE_LOG(ObLogSaveSystem, Error, TEXT("Failed to create or load Master Save Object for LocalPlayer!"));
}

void UObsidianSaveGameSubsystem::AsyncLoadOrCreateSharedStashDataSaveObject(const UObsidianLocalPlayer* InLocalPlayer,
	const bool bInOnline)
{
	if (ensure(InLocalPlayer))
	{
		UE_LOG(ObLogSaveSystem, Log, TEXT("Creating or loading existing [%s] Shared Stash Data for [%s]."),
			bInOnline ? TEXT("Online") : TEXT("Offline"), *GetNameSafe(InLocalPlayer));

		bool bSuccess = false;
		if (bInOnline)
		{
			UObsidianSharedStashSaveGame::AsyncLoadOrCreateSaveGameForLocalPlayer(
				UObsidianSharedStashSaveGame::StaticClass(), InLocalPlayer, ObsidianSaveStatics::OnlineStashDataSaveName,
				FOnLocalPlayerSaveGameLoadedNative::CreateWeakLambda(this, [this](ULocalPlayerSaveGame* InSaveGame)
					{
						check(InSaveGame);
						OnlineSharedStashData = Cast<UObsidianSharedStashSaveGame>(InSaveGame);
						OnSharedStashDataLoadedDelegate.Broadcast(OnlineSharedStashData);
					}));
		}
		else
		{
			UObsidianSharedStashSaveGame::AsyncLoadOrCreateSaveGameForLocalPlayer(
				UObsidianSharedStashSaveGame::StaticClass(), InLocalPlayer, ObsidianSaveStatics::OfflineStashDataSaveName,
				FOnLocalPlayerSaveGameLoadedNative::CreateWeakLambda(this, [this](ULocalPlayerSaveGame* InSaveGame)
					{
						check(InSaveGame);
						OfflineSharedStashData = Cast<UObsidianSharedStashSaveGame>(InSaveGame);
						OnSharedStashDataLoadedDelegate.Broadcast(OfflineSharedStashData);
					}));
		}
		return;
	}
	
	UE_LOG(ObLogSaveSystem, Error, TEXT("Failed to create or load Master Save Object for LocalPlayer!"));
}

void UObsidianSaveGameSubsystem::RegisterSaveable(AActor* InSaveActor)
{
	if (ensure(InSaveActor && InSaveActor->GetClass()->ImplementsInterface(UObsidianSaveableInterface::StaticClass())))
	{
		SaveableActors.AddUnique(InSaveActor);
	}
}

void UObsidianSaveGameSubsystem::UnregisterSaveable(AActor* InSaveActor)
{
	SaveableActors.RemoveSingleSwap(InSaveActor);
}

void UObsidianSaveGameSubsystem::RequestSaveGame(const UObsidianLocalPlayer* InLocalPlayer, const bool bInAsync)
{
	if (ensure(InLocalPlayer) && ensure(CurrentHeroSaveGame) && ensure(ObsidianMasterSaveGame))
	{
		UE_LOG(ObLogSaveSystem, Log, TEXT("Requested Save for [%s]. "), *GetNameSafe(InLocalPlayer));
		
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
		
			
		if (bInAsync)
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

void UObsidianSaveGameSubsystem::RequestSaveInitialHeroSave(const UObsidianLocalPlayer* InLocalPlayer, const bool bInAsync,
	const bool bInOnline, const FObsidianHeroInitializationSaveData& InHeroInitializationSaveData)
{
	if (ensure(InLocalPlayer) && ensure(ObsidianMasterSaveGame))
	{
		UE_LOG(ObLogSaveSystem, Log, TEXT("Requested Initial Hero Save for [%s]. "),
			*GetNameSafe(InLocalPlayer));
		
		const FObsidianAddHeroSaveResult Result = ObsidianMasterSaveGame->AddHero(bInOnline, InHeroInitializationSaveData);
		UObsidianHeroSaveGame* NewHeroSaveGame = CreateHeroSaveGameObject(InLocalPlayer, Result.SaveName, Result.SaveID);
		NewHeroSaveGame->InitializeHeroSaveData(bInOnline, InHeroInitializationSaveData);
		CurrentHeroSaveGame = NewHeroSaveGame;

		//TODO(intrxx) Make it async 
		ObsidianMasterSaveGame->SaveGameToSlotForLocalPlayer();
		
		if (bInAsync)
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

void UObsidianSaveGameSubsystem::AsyncSaveSharedStashData(const AObsidianPlayerController* InPlayerController,
	const EObsidianGameNetworkType InNetworkType)
{
	if (InPlayerController == nullptr)
	{
		UE_LOG(ObLogSaveSystem, Error, TEXT("Provided PlayerController is invalid in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = InPlayerController->GetPlayerStashComponent();
	if (PlayerStashComponent == nullptr)
	{
		UE_LOG(ObLogSaveSystem, Error, TEXT("PlayerStashComponent is invalid in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianSharedStashSaveGame* SharedStashSaveGame = UObsidianGameplayStatics::IsOfflineNetworkType(InNetworkType)
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

UObsidianHeroSaveGame* UObsidianSaveGameSubsystem::CreateHeroSaveGameObject(const UObsidianLocalPlayer* InLocalPlayer,
                                                                            const FString& InSlotName, const uint16 InSaveID)
{
	if (ensure(InLocalPlayer))
	{
		UE_LOG(ObLogSaveSystem, Log, TEXT("Creating Save Object for [%s]. "),
			*GetNameSafe(InLocalPlayer));
		
		if (ULocalPlayerSaveGame* LocalSaveGame = UObsidianHeroSaveGame::CreateNewSaveGameForLocalPlayer(
			UObsidianHeroSaveGame::StaticClass(), InLocalPlayer, InSlotName))
		{
			UObsidianHeroSaveGame* ObsidianHeroSaveGame = Cast<UObsidianHeroSaveGame>(LocalSaveGame);
			ObsidianHeroSaveGame->InitWithSaveSystem(this);
			ObsidianHeroSaveGame->SetSaveID(InSaveID);
			return ObsidianHeroSaveGame;
		}
	}

	UE_LOG(ObLogSaveSystem, Error, TEXT("Failed to create Save Object for LocalPlayer!"));
	return nullptr;
}

void UObsidianSaveGameSubsystem::RequestLoadHeroSaveGameWithID(const UObsidianLocalPlayer* InLocalPlayer, const bool bInAsync,
	const uint16 InSaveID, const bool bInOnline)
{
	if (ensure(InLocalPlayer))
	{
		check(ObsidianMasterSaveGame)
		const FString HeroSaveName = ObsidianMasterSaveGame->GetSaveNameForID(InSaveID, bInOnline);
		RequestLoadGame(InLocalPlayer, bInAsync, HeroSaveName);
	}
}

void UObsidianSaveGameSubsystem::RequestLoadGame(const UObsidianLocalPlayer* InLocalPlayer, const bool bInAsync,
	const FString& InSlotName)
{
	if (ensure(InLocalPlayer))
	{
		UE_LOG(ObLogSaveSystem, Log, TEXT("Requested Load Game for [%s]. "),
			*GetNameSafe(InLocalPlayer));
		
		if (bInAsync)
		{
			LoadGameForPlayerAsync(InLocalPlayer, InSlotName);
		}
		else
		{
			LoadGameForPlayer(InLocalPlayer, InSlotName);
		}
		return;
	}

	UE_LOG(ObLogSaveSystem, Error, TEXT("Failed to perform load Hero Save with invalid Local Player. "));
	OnLoadingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::RequestLoadDataForObject(AActor* InLoadActor)
{
	if (IObsidianSaveableInterface* SaveableInterface = Cast<IObsidianSaveableInterface>(InLoadActor))
	{
		SaveableInterface->LoadData(CurrentHeroSaveGame);
	}
}

bool UObsidianSaveGameSubsystem::DeleteHeroSave(const uint16 InSaveID, const bool bInOnline)
{
	check(ObsidianMasterSaveGame)
	const FString SlotNameToDelete = ObsidianMasterSaveGame->GetSaveNameForID(InSaveID, bInOnline);
	const int32 UserIndex = ObsidianMasterSaveGame->GetPlatformUserIndex();
	const bool bSuccess = UGameplayStatics::DeleteGameInSlot(SlotNameToDelete, UserIndex);
	if (bSuccess)
	{
		if (ObsidianMasterSaveGame->DeleteHero(InSaveID, bInOnline))
		{
			ObsidianMasterSaveGame->AsyncSaveGameToSlotForLocalPlayer();
		}
		else
		{
			UE_LOG(ObLogSaveSystem, Error, TEXT("[%s] save with id [%d], of retrieved name [%s],"
				" could not be deleted on Master Save Object."), bInOnline ? TEXT("Online") : TEXT("Offline"), InSaveID,
				*SlotNameToDelete)
		}

		if (CurrentHeroSaveGame && CurrentHeroSaveGame->GetSaveID() == InSaveID && CurrentHeroSaveGame->IsOnline() == bInOnline)
		{
			CurrentHeroSaveGame = nullptr;
		}
	}
	else
	{
		if (!UGameplayStatics::DoesSaveGameExist(SlotNameToDelete, UserIndex))
		{
			UE_LOG(ObLogSaveSystem, Error, TEXT("[%s] save with id [%d], of retrieved name [%s] does not exist,"
				" and could not be deleted."), bInOnline ? TEXT("Online") : TEXT("Offline"), InSaveID, *SlotNameToDelete)
		}
		else
		{
			UE_LOG(ObLogSaveSystem, Error, TEXT("[%s] save with id [%d], of retrieved name [%s], could not be deleted."),
				bInOnline ? TEXT("Online") : TEXT("Offline"), InSaveID, *SlotNameToDelete)
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

void UObsidianSaveGameSubsystem::LoadGameForPlayer(const UObsidianLocalPlayer* InLocalPlayer, const FString& InSlotName)
{
	UObsidianHeroSaveGame::LoadOrCreateSaveGameForLocalPlayer(UObsidianHeroSaveGame::StaticClass(), InLocalPlayer,
		InSlotName);
}

void UObsidianSaveGameSubsystem::LoadGameForPlayerAsync(const UObsidianLocalPlayer* InLocalPlayer, const FString& InSlotName)
{
	UObsidianHeroSaveGame::AsyncLoadOrCreateSaveGameForLocalPlayer(UObsidianHeroSaveGame::StaticClass(), InLocalPlayer,
		InSlotName,FOnLocalPlayerSaveGameLoadedNative::CreateLambda([this](ULocalPlayerSaveGame* InSaveGame)
			{
				HandleLoadingHeroSaveFinished(Cast<UObsidianHeroSaveGame>(InSaveGame));
			}));
}

void UObsidianSaveGameSubsystem::HandleLoadingHeroSaveFinished(UObsidianHeroSaveGame* InSaveGame)
{
	if (InSaveGame)
	{
		CurrentHeroSaveGame = InSaveGame;
		OnLoadingFinishedDelegate.Broadcast(InSaveGame, true);
		return;
	}
	OnLoadingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::HandleSavingHeroSaveFinished(const bool bInSuccess, UObsidianHeroSaveGame* InSaveGame)
{
	if (bInSuccess && InSaveGame)
	{
		OnSavingFinishedDelegate.Broadcast(InSaveGame, true);
		return;
	}
	OnSavingFinishedDelegate.Broadcast(nullptr, false);
}

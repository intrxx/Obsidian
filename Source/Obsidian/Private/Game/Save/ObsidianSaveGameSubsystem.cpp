// Copyright 2024 out of sCope team - Michał Ogiński

#include "Game/Save/ObsidianSaveGameSubsystem.h"

#include <Kismet/GameplayStatics.h>

#include "Game/Save/ObsidianSaveableInterface.h"
#include "Game/Save/ObsidianSaveGame.h"

UObsidianSaveGame* UObsidianSaveGameSubsystem::GetSaveGameObject()
{
	return ObsidianSaveGame;
}

void UObsidianSaveGameSubsystem::CreateSaveGameObject()
{
	SaveSlotName = FString("MySlotName");
	ObsidianSaveGame = Cast<UObsidianSaveGame>(UGameplayStatics::CreateSaveGameObject(UObsidianSaveGame::StaticClass()));
}

void UObsidianSaveGameSubsystem::RegisterSaveable(AActor* SaveActor)
{
	check(!SaveableActors.Contains(SaveActor) && SaveActor->GetClass()->ImplementsInterface(UObsidianSaveableInterface::StaticClass()))
	SaveableActors.Add(SaveActor);
}

void UObsidianSaveGameSubsystem::UnregisterSaveable(AActor* SaveActor)
{
	check(SaveableActors.Contains(SaveActor))
	SaveableActors.Remove(SaveActor);
}

void UObsidianSaveGameSubsystem::RequestSaveGame(const bool bAsync)
{
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
		SaveGameForPlayerAsync();
		return;
	}
	SaveGameForPlayer();
}

void UObsidianSaveGameSubsystem::RequestSaveInitialHeroSave(const bool bAsync, const FObsidianHeroInitializationSaveData& HeroInitializationSaveData)
{
	if (ObsidianSaveGame)
	{
		ObsidianSaveGame->InitializeHeroSaveData(HeroInitializationSaveData);
	}
	
	if (bAsync)
	{
		SaveGameForPlayerAsync();
		return;
	}
	SaveGameForPlayer();
}

void UObsidianSaveGameSubsystem::RequestLoadGame(const bool bAsync)
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		if (bAsync)
		{
			LoadGameForPlayerAsync();
		}
		else
		{
			LoadGameForPlayer();
		}
		return;
	}
	OnLoadingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::RequestLoadDataForObject(AActor* LoadActor)
{
	if (IObsidianSaveableInterface* SaveableInterface = Cast<IObsidianSaveableInterface>(LoadActor))
	{
		SaveableInterface->LoadData(ObsidianSaveGame);
	}
}

void UObsidianSaveGameSubsystem::SaveGameForPlayer()
{
	if (UGameplayStatics::SaveGameToSlot(ObsidianSaveGame, SaveSlotName, 0))
	{
		OnSavingFinishedDelegate.Broadcast(ObsidianSaveGame, true);
		return;
	}
	OnSavingFinishedDelegate.Broadcast(nullptr, false);
}

void UObsidianSaveGameSubsystem::SaveGameForPlayerAsync()
{
	UGameplayStatics::AsyncSaveGameToSlot(ObsidianSaveGame, SaveSlotName, 0,
		FAsyncSaveGameToSlotDelegate::CreateLambda([this](const FString& SlotName, int32 UserIndex, bool bSuccess)
			{
				if (bSuccess)
				{
					OnSavingFinishedDelegate.Broadcast(ObsidianSaveGame, true);
					return;
				}
				OnSavingFinishedDelegate.Broadcast(nullptr, false);
			}));
}

void UObsidianSaveGameSubsystem::LoadGameForPlayer()
{
	ObsidianSaveGame = Cast<UObsidianSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if (ObsidianSaveGame)
	{
		OnLoadingFinishedDelegate.Broadcast(ObsidianSaveGame, true);
		return;
	}
	OnLoadingFinishedDelegate.Broadcast(ObsidianSaveGame, false);
}

void UObsidianSaveGameSubsystem::LoadGameForPlayerAsync()
{
	UE_LOG(LogTemp, Warning, TEXT("LoadGameForPlayerAsync CALLED"));
	
	UGameplayStatics::AsyncLoadGameFromSlot(SaveSlotName, 0,
		FAsyncLoadGameFromSlotDelegate::CreateLambda([this](const FString& SlotName, const int32 UserIndex, USaveGame* SaveGame)
			{
				if (SaveGame)
				{
					ObsidianSaveGame = Cast<UObsidianSaveGame>(SaveGame);
					if (ObsidianSaveGame)
					{
						OnLoadingFinishedDelegate.Broadcast(ObsidianSaveGame, true);
						return;
					}
					OnLoadingFinishedDelegate.Broadcast(nullptr, false);
				}
			}));
}

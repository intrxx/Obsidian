// Copyright 2026 out of sCope team - intrxx

#include "Game/Save/ObsidianMasterSaveGame.h"

FObsidianAddHeroSaveResult UObsidianMasterSaveGame::AddHero(const bool bOnline,
                                                            const FObsidianHeroInitializationSaveData& HeroSaveData)
{
	if (bOnline)
	{
		return AddOnlineHero(HeroSaveData);
	}
	return AddOfflineHero(HeroSaveData);
}

bool UObsidianMasterSaveGame::DeleteHero(const uint16 SaveID, const bool bOnline)
{
	if (bOnline)
	{
		for(auto It = MasterSaveParams.OnlineSavedHeroes.CreateIterator(); It; ++It)
		{
			FObsidianHeroSaveInfo& Params = *It;
			if(Params.SaveID == SaveID)
			{
				It.RemoveCurrent();
				return true;
			}
		}
		return false;
	}
	
	for(auto It = MasterSaveParams.OfflineSavedHeroes.CreateIterator(); It; ++It)
	{
		FObsidianHeroSaveInfo& Params = *It;
		if(Params.SaveID == SaveID)
		{
			It.RemoveCurrent();
			return true;
		}
	}
	return false;
}

bool UObsidianMasterSaveGame::UpdateHeroSave(const uint16 SaveID, const bool bOnline, const uint8 HeroLevel)
{
	if (FObsidianHeroSaveInfo* SaveInfo = GetHeroSaveInfo(SaveID, bOnline))
	{
		SaveInfo->HeroDescription.HeroLevel = HeroLevel;
		return true;
	}
	return false;
}

TArray<FObsidianHeroSaveInfo> UObsidianMasterSaveGame::GetHeroSaveInfos(const bool bOnline)
{
	if (bOnline)
	{
		return MasterSaveParams.OnlineSavedHeroes;
	}
	return MasterSaveParams.OfflineSavedHeroes;
}

FString UObsidianMasterSaveGame::GetSaveNameForID(const uint16 SaveID, const bool bOnline) const
{
	if (bOnline)
	{
		for (const FObsidianHeroSaveInfo& SaveInfo : MasterSaveParams.OnlineSavedHeroes)
		{
			if (SaveInfo.SaveID == SaveID)
			{
				return SaveInfo.SaveName;
			}
		}
		
		return FString();
	}

	for (const FObsidianHeroSaveInfo& SaveInfo : MasterSaveParams.OfflineSavedHeroes)
	{
		if (SaveInfo.SaveID == SaveID)
		{
			return SaveInfo.SaveName;
		}
	}
		
	return FString();
}

FObsidianAddHeroSaveResult UObsidianMasterSaveGame::AddOfflineHero(const FObsidianHeroInitializationSaveData& HeroSaveData)
{
	FObsidianHeroSaveInfo HeroSaveInfo;
	HeroSaveInfo.SaveID = GetMaxOfflineSaveID();
	HeroSaveInfo.SaveName = FString::Printf(TEXT("offline_hero_%d"), HeroSaveInfo.SaveID), 
	HeroSaveInfo.bOnline = false;

	FObsidianHeroDescription HeroDescription;
	HeroDescription.HeroName = HeroSaveData.PlayerHeroName;
	HeroDescription.HeroClass = HeroSaveData.HeroClass;
	HeroDescription.HeroLevel = 1;
	HeroDescription.bHardcore = HeroSaveData.bHardcore;

	HeroSaveInfo.HeroDescription = HeroDescription;
	MasterSaveParams.OfflineSavedHeroes.Add(HeroSaveInfo);

	return FObsidianAddHeroSaveResult(HeroSaveInfo.SaveName, HeroSaveInfo.SaveID);
}

FObsidianAddHeroSaveResult UObsidianMasterSaveGame::AddOnlineHero(const FObsidianHeroInitializationSaveData& HeroSaveData)
{
	FObsidianHeroSaveInfo HeroSaveInfo;
	HeroSaveInfo.SaveID = GetMaxOnlineSaveID();
	HeroSaveInfo.SaveName = FString::Printf(TEXT("online_hero_%d"), HeroSaveInfo.SaveID);
	HeroSaveInfo.bOnline = true;

	FObsidianHeroDescription HeroDescription;
	HeroDescription.HeroName = HeroSaveData.PlayerHeroName;
	HeroDescription.HeroClass = HeroSaveData.HeroClass;
	HeroDescription.HeroLevel = 1;
	HeroDescription.bHardcore = HeroSaveData.bHardcore;

	HeroSaveInfo.HeroDescription = HeroDescription;
	MasterSaveParams.OnlineSavedHeroes.Add(HeroSaveInfo);

	return FObsidianAddHeroSaveResult(HeroSaveInfo.SaveName, HeroSaveInfo.SaveID);
}

FObsidianHeroSaveInfo* UObsidianMasterSaveGame::GetHeroSaveInfo(const uint16 SaveID, const bool bOnline)
{
	if (bOnline)
	{
		for (FObsidianHeroSaveInfo& SaveInfo : MasterSaveParams.OnlineSavedHeroes)
		{
			if (SaveInfo.SaveID == SaveID)
			{
				return &SaveInfo;
			}
		}

		return nullptr;
	}
	
	for (FObsidianHeroSaveInfo& SaveInfo : MasterSaveParams.OfflineSavedHeroes)
	{
		if (SaveInfo.SaveID == SaveID)
		{
			return &SaveInfo;
		}
	}

	return nullptr;
}

uint16 UObsidianMasterSaveGame::GetMaxOfflineSaveID() const
{
	return MasterSaveParams.OfflineSavedHeroes.Num();
}

uint16 UObsidianMasterSaveGame::GetMaxOnlineSaveID() const
{
	return MasterSaveParams.OnlineSavedHeroes.Num();
}

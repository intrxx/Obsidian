// Copyright 2026 out of sCope team - intrxx

#include "Game/Save/ObsidianMasterSaveGame.h"


FObsidianAddHeroSaveResult UObsidianMasterSaveGame::AddHero(const bool bInOnline,
                                                            const FObsidianHeroInitializationSaveData& InHeroSaveData)
{
	if (bInOnline)
	{
		return AddOnlineHero(InHeroSaveData);
	}
	return AddOfflineHero(InHeroSaveData);
}

bool UObsidianMasterSaveGame::DeleteHero(const uint16 InSaveID, const bool bInOnline)
{
	if (bInOnline)
	{
		for(auto It = MasterSaveParams.OnlineSavedHeroes.CreateIterator(); It; ++It)
		{
			FObsidianHeroSaveInfo& Params = *It;
			if(Params.SaveID == InSaveID)
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
		if(Params.SaveID == InSaveID)
		{
			It.RemoveCurrent();
			return true;
		}
	}
	return false;
}

bool UObsidianMasterSaveGame::UpdateHeroSave(const uint16 InSaveID, const bool bInOnline, const uint8 InHeroLevel)
{
	if (FObsidianHeroSaveInfo* SaveInfo = GetHeroSaveInfo(InSaveID, bInOnline))
	{
		SaveInfo->HeroDescription.HeroLevel = InHeroLevel;
		return true;
	}
	return false;
}

TArray<FObsidianHeroSaveInfo> UObsidianMasterSaveGame::GetHeroSaveInfos(const bool bInOnline)
{
	if (bInOnline)
	{
		return MasterSaveParams.OnlineSavedHeroes;
	}
	return MasterSaveParams.OfflineSavedHeroes;
}

FString UObsidianMasterSaveGame::GetSaveNameForID(const uint16 InSaveID, const bool bInOnline) const
{
	if (bInOnline)
	{
		for (const FObsidianHeroSaveInfo& SaveInfo : MasterSaveParams.OnlineSavedHeroes)
		{
			if (SaveInfo.SaveID == InSaveID)
			{
				return SaveInfo.SaveName;
			}
		}
		
		return FString();
	}

	for (const FObsidianHeroSaveInfo& SaveInfo : MasterSaveParams.OfflineSavedHeroes)
	{
		if (SaveInfo.SaveID == InSaveID)
		{
			return SaveInfo.SaveName;
		}
	}
		
	return FString();
}

FObsidianAddHeroSaveResult UObsidianMasterSaveGame::AddOfflineHero(const FObsidianHeroInitializationSaveData& InHeroSaveData)
{
	FObsidianHeroSaveInfo HeroSaveInfo;
	HeroSaveInfo.SaveID = GetMaxOfflineSaveID();
	HeroSaveInfo.SaveName = FString::Printf(TEXT("offline_hero_%d"), HeroSaveInfo.SaveID), 
	HeroSaveInfo.bOnline = false;

	FObsidianHeroDescription HeroDescription;
	HeroDescription.HeroName = InHeroSaveData.PlayerHeroName;
	HeroDescription.HeroClass = InHeroSaveData.HeroClass;
	HeroDescription.HeroLevel = 1;
	HeroDescription.bHardcore = InHeroSaveData.bHardcore;

	HeroSaveInfo.HeroDescription = HeroDescription;
	MasterSaveParams.OfflineSavedHeroes.Add(HeroSaveInfo);

	return FObsidianAddHeroSaveResult(HeroSaveInfo.SaveName, HeroSaveInfo.SaveID);
}

FObsidianAddHeroSaveResult UObsidianMasterSaveGame::AddOnlineHero(const FObsidianHeroInitializationSaveData& InHeroSaveData)
{
	FObsidianHeroSaveInfo HeroSaveInfo;
	HeroSaveInfo.SaveID = GetMaxOnlineSaveID();
	HeroSaveInfo.SaveName = FString::Printf(TEXT("online_hero_%d"), HeroSaveInfo.SaveID);
	HeroSaveInfo.bOnline = true;

	FObsidianHeroDescription HeroDescription;
	HeroDescription.HeroName = InHeroSaveData.PlayerHeroName;
	HeroDescription.HeroClass = InHeroSaveData.HeroClass;
	HeroDescription.HeroLevel = 1;
	HeroDescription.bHardcore = InHeroSaveData.bHardcore;

	HeroSaveInfo.HeroDescription = HeroDescription;
	MasterSaveParams.OnlineSavedHeroes.Add(HeroSaveInfo);

	return FObsidianAddHeroSaveResult(HeroSaveInfo.SaveName, HeroSaveInfo.SaveID);
}

FObsidianHeroSaveInfo* UObsidianMasterSaveGame::GetHeroSaveInfo(const uint16 InSaveID, const bool bInOnline)
{
	if (bInOnline)
	{
		for (FObsidianHeroSaveInfo& SaveInfo : MasterSaveParams.OnlineSavedHeroes)
		{
			if (SaveInfo.SaveID == InSaveID)
			{
				return &SaveInfo;
			}
		}

		return nullptr;
	}
	
	for (FObsidianHeroSaveInfo& SaveInfo : MasterSaveParams.OfflineSavedHeroes)
	{
		if (SaveInfo.SaveID == InSaveID)
		{
			return &SaveInfo;
		}
	}

	return nullptr;
}

uint16 UObsidianMasterSaveGame::GetMaxOfflineSaveID() const
{
	uint16 NextSaveID = 0;
	for (const FObsidianHeroSaveInfo& SaveInfo : MasterSaveParams.OfflineSavedHeroes)
	{
		NextSaveID = FMath::Max<uint16>(NextSaveID, SaveInfo.SaveID + 1);
	}
	return NextSaveID;
}

uint16 UObsidianMasterSaveGame::GetMaxOnlineSaveID() const
{
	uint16 NextSaveID = 0;
	for (const FObsidianHeroSaveInfo& SaveInfo : MasterSaveParams.OnlineSavedHeroes)
	{
		NextSaveID = FMath::Max<uint16>(NextSaveID, SaveInfo.SaveID + 1);
	}
	return NextSaveID;
}

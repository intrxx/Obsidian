// Copyright 2024 out of sCope team - Michał Ogiński

#include "Game/Save/ObsidianSaveGame.h"


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

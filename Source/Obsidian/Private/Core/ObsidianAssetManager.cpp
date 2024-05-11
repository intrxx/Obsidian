// Copyright 2024 out of sCope team - Michał Ogiński


#include "Core/ObsidianAssetManager.h"

UObsidianAssetManager& UObsidianAssetManager::Get()
{
	check(GEngine);
	UObsidianAssetManager* ObsidianAssetManager = Cast<UObsidianAssetManager>(GEngine->AssetManager);
	
	return *ObsidianAssetManager;
}

void UObsidianAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	
}

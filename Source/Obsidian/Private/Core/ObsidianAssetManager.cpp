// Copyright 2026 out of sCope team - intrxx

#include "Core/ObsidianAssetManager.h"

// ~ Core

// ~ Project

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

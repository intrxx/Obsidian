// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "ObsidianAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	virtual void StartInitialLoading() override;
};
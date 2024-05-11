// Copyright 2024 out of sCope team - Michał Ogiński

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
	static UObsidianAssetManager& Get();
	
	virtual void StartInitialLoading() override;
};

// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


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

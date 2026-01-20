// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>


#include <UObject/Interface.h>
#include "ObsidianSaveableInterface.generated.h"

class UObsidianHeroSaveGame;

UINTERFACE(MinimalAPI)
class UObsidianSaveableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OBSIDIAN_API IObsidianSaveableInterface
{
	GENERATED_BODY()

public:
	virtual void SaveData(UObsidianHeroSaveGame* SaveObject) = 0;
	virtual void LoadData(UObsidianHeroSaveGame* SaveObject) = 0;
};

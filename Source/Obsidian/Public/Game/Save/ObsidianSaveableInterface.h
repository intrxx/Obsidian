// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>


#include <UObject/Interface.h>
#include "ObsidianSaveableInterface.generated.h"

class UObsidianSaveGame;

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
	virtual void SaveData(UObsidianSaveGame* SaveObject) = 0;
	virtual void LoadData(UObsidianSaveGame* SaveObject) = 0;
};

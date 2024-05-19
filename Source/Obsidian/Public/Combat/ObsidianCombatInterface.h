// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ObsidianCombatInterface.generated.h"

UINTERFACE(MinimalAPI)
class UObsidianCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OBSIDIAN_API IObsidianCombatInterface
{
	GENERATED_BODY()

public:
	virtual int32 GetCharacterLevel();
	virtual FVector GetCombatSocketLocationFromWeapon();
};

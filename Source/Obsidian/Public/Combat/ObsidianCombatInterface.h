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

	/*
	 * Socket Locations for spawning abilities.
	 */

	/** Gets the socket location from left hand weapon. */
	virtual FVector GetAbilitySocketLocationFromLHWeapon();

	/** Gets the socket location from right hand weapon. */
	virtual FVector GetAbilitySocketLocationFromRHWeapon();

	virtual FVector GetAbilitySocketLocationFromLeftHand();
	virtual FVector GetAbilitySocketLocationFromRightHand();

	/** Gets the default location for spawning ability which is slightly ahead the player character. */
	virtual FVector GetAbilityDefaultLocation();

	/*
	 * 
	 */
};

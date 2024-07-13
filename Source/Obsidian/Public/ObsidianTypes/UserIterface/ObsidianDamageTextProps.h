// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianDamageTextProps.generated.h"

USTRUCT(BlueprintType)
struct FObsidianDamageTextProps
{
	GENERATED_BODY()

	float DamageMagnitude = 0.0f;
	
	bool bIsBlockedAttack = false;
	bool bIsCriticalAttack = false;
	bool bIsEvadedHit = false;
	bool bIsSuppressedSpell = false;
};


// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianDamageTextProps.generated.h"

USTRUCT(BlueprintType)
struct FObsidianDamageTextProps
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float DamageMagnitude = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsBlockedAttack = false;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsCriticalAttack = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsEvadedHit = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsSuppressedSpell = false;
};


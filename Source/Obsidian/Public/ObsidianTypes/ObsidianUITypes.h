// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "ObsidianUITypes.generated.h"

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

USTRUCT(BlueprintType)
struct FObsidianEffectUIStackingData
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(BlueprintReadOnly)
	int32 EffectStackCount = 0;

	UPROPERTY(BlueprintReadOnly)
	EGameplayEffectStackingExpirationPolicy EffectExpirationDurationPolicy = EGameplayEffectStackingExpirationPolicy();

	UPROPERTY(BlueprintReadOnly)
	EGameplayEffectStackingDurationPolicy EffectStackingDurationPolicy = EGameplayEffectStackingDurationPolicy();
};

UENUM(BlueprintType)
enum class EObsidianUIEffectClassification : uint8
{
	EUEC_NoClassification UMETA(DisplayName = "No Classification"), 
	EUEC_Buff UMETA(DisplayName = "Buff"), 
	EUEC_Debuff UMETA(DisplayName = "Debuff"), 

	EUEC_MAX UMETA(DisplayName = "Default MAX") 
};
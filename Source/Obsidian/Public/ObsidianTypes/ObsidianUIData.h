#pragma once

#include "GameplayEffect.h"
#include "ObsidianUIData.generated.h"

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


// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "AbilitySystemGlobals.h"
#include "CoreMinimal.h"

#include "ObsidianAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	//~ Begin UAbilitySystemGlobals Interface
	/** Should allocate a project specific GameplayEffectContext struct. Caller is responsible for deallocation */
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	//~ End UAbilitySystemGlobals Interface
};

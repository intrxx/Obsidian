// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "ObsidianAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FObsidianGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY();

public:
	FObsidianGameplayEffectContext()
	{
		
	}

	/** Returns true if hit was blocked, it is not determined if it was a spell or hit */
	bool IsBlockedAttack() const
	{
		return bIsBlockedAttack;
	}

	/** Returns true if hit was a critical strike */
	bool IsCriticalHit() const
	{
		return bIsCriticalHit;
	}
	
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayEffectContext::StaticStruct();
	}

	void SetIsBlockedAttack(const bool bInIsBlockedAttack)
	{
		bIsBlockedAttack = bInIsBlockedAttack;
	}
	
	void SetIsCriticalHit(const bool bInIsCriticalHit)
	{
		bIsCriticalHit = bInIsCriticalHit;
	}

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
	
protected:
	UPROPERTY()
	bool bIsBlockedAttack = false;
	
	UPROPERTY()
	bool bIsCriticalHit = false;
};
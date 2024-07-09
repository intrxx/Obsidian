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
		return StaticStruct();
	}

	void SetIsBlockedAttack(const bool bInIsBlockedAttack)
	{
		bIsBlockedAttack = bInIsBlockedAttack;
	}
	
	void SetIsCriticalHit(const bool bInIsCriticalHit)
	{
		bIsCriticalHit = bInIsCriticalHit;
	}

	/** Creates a copy of this context, used to duplicate for later modifications */ // I'm not sure if override should be here since I changed the return type
	virtual FObsidianGameplayEffectContext* Duplicate() const override
	{
		FObsidianGameplayEffectContext* NewContext = new FObsidianGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
	
protected:
	UPROPERTY()
	bool bIsBlockedAttack = false;
	
	UPROPERTY()
	bool bIsCriticalHit = false;
};

template<>
struct TStructOpsTypeTraits<FObsidianGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FObsidianGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "ObsidianAbilitySystemEffectTypes.generated.h"

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

	/** Returns true if attack, was a critical strike */
	bool IsCriticalAttack() const
	{
		return bIsCriticalAttack;
	}

	/** Returns true if hit, was evaded or not */
	bool IsEvadedHit() const
	{
		return bIsEvadedHit;
	}

	/** Returns true if spell, was suppressed or not */
	bool IsSuppressedSpell() const
	{
		return bIsSuppressedSpell;
	}
	
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}

	/** Set whether the Attack was blocked or not */
	void SetIsBlockedAttack(const bool bInIsBlockedAttack)
	{
		bIsBlockedAttack = bInIsBlockedAttack;
	}

	/** Set whether the Attack was critical or not */
	void SetIsCriticalAttack(const bool bInIsCriticalAttack)
	{
		bIsCriticalAttack = bInIsCriticalAttack;
	}

	/** Set whether the hit was evaded or not */
	void SetIsEvadedHit(const bool bInIsEvadedHit)
	{
		bIsEvadedHit = bInIsEvadedHit;
	}

	/** Set whether the spell was suppressed or not */
	void SetIsSuppressedSpell(const bool bInIsSuppressedSpell)
	{
		bIsSuppressedSpell = bInIsSuppressedSpell;
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
	bool bIsCriticalAttack = false;

	UPROPERTY()
	bool bIsEvadedHit = false;

	UPROPERTY()
	bool bIsSuppressedSpell = false;
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

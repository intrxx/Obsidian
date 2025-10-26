// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/ObsidianAbilitySystemEffectTypes.h"

// ~ Core

// ~ Project

bool FObsidianEffectAffixValue::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	uint16 RepBits = 0;
	if (Ar.IsSaving())
	{
		if (AffixTag != FGameplayTag::EmptyTag)
		{
			RepBits |= 1 << 0;
		}
		if (CurrentAffixValues.Num() > 0)
		{
			RepBits |= 1 << 1;
		}
	}

	Ar.SerializeBits(&RepBits, 2);

	if (RepBits & (1 << 0))
	{
		Ar << AffixTag;
	}
	if (RepBits & (1 << 1))
	{
		SafeNetSerializeTArray_Default<2>(Ar, CurrentAffixValues);
	}

	bOutSuccess = true;
	return true;
}

FObsidianGameplayEffectContext* FObsidianGameplayEffectContext::ExtractEffectContextFromHandle(FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if(((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(StaticStruct())))
	{
		return (FObsidianGameplayEffectContext*)BaseEffectContext;
	}
	return nullptr;
}

void FObsidianGameplayEffectContext::InitializeAffixValues(const TArray<FObsidianEffectAffixValue>& EffectAffixValues)
{
	AffixValues.Append(EffectAffixValues);
}

bool FObsidianGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	uint32 RepBits = 0;
	if (Ar.IsSaving())
	{
		if (bReplicateInstigator && Instigator.IsValid())
		{
			// This is equivalent to RepBits = RepBits | 1 << 0;
			RepBits |= 1 << 0;
		}
		if (bReplicateEffectCauser && EffectCauser.IsValid() )
		{
			RepBits |= 1 << 1;
		}
		if (AbilityCDO.IsValid())
		{
			RepBits |= 1 << 2;
		}
		if (bReplicateSourceObject && SourceObject.IsValid())
		{
			RepBits |= 1 << 3;
		}
		if (Actors.Num() > 0)
		{
			RepBits |= 1 << 4;
		}
		if (HitResult.IsValid())
		{
			RepBits |= 1 << 5;
		}
		if (bHasWorldOrigin)
		{
			RepBits |= 1 << 6;
		}
		if (bIsBlockedAttack)
		{
			RepBits |= 1 << 7;
		}
		if (bIsCriticalAttack)
		{
			RepBits |= 1 << 8;
		}
		if(bIsEvadedHit)
		{
			RepBits |= 1 << 9;
		}
		if(bIsSuppressedSpell)
		{
			RepBits |= 1 << 10;
		}
		if(bIsTargetImmune)
		{
			RepBits |= 1 << 11;
		}
		if (AffixValues.Num() > 0)
		{
			RepBits |= 1 << 12;
		}
	}

	Ar.SerializeBits(&RepBits, 13);

	if (RepBits & (1 << 0))
	{
		Ar << Instigator;
	}
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;
	}
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO;
	}
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject;
	}
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}
	if (RepBits & (1 << 7))
	{
		Ar << bIsBlockedAttack;
	}
	if(RepBits & (1 << 8))
	{
		Ar << bIsCriticalAttack;
	}
	if(RepBits & (1 << 9))
	{
		Ar << bIsEvadedHit;
	}
	if(RepBits & (1 << 10))
	{
		Ar << bIsSuppressedSpell;
	}
	if(RepBits & (1 << 11))
	{
		Ar << bIsTargetImmune;
	}
	if (RepBits & (1 << 12))
	{
#if UE_BUILD_SHIPPING
		SafeNetSerializeTArray_Default<8>(Ar, AffixValues)
#else
		if (SafeNetSerializeTArray_WithNetSerialize<8>(Ar, AffixValues, Map) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("Array serialization of AffixValues in FObsidianGameplayEffectContext failed!"))
		}
#endif
	}

	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
	}	
	
	bOutSuccess = true;
	return true;
}

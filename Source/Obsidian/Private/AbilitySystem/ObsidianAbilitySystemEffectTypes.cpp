// Copyright 2026 out of sCope team - intrxx

#include "AbilitySystem/ObsidianAbilitySystemEffectTypes.h"


FObsidianGameplayEffectContext* FObsidianGameplayEffectContext::ExtractEffectContextFromHandle(
	FGameplayEffectContextHandle InHandle)
{
	FGameplayEffectContext* BaseEffectContext = InHandle.Get();
	if(((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(StaticStruct())))
	{
		return (FObsidianGameplayEffectContext*)BaseEffectContext;
	}
	return nullptr;
}

bool FObsidianGameplayEffectContext::NetSerialize(FArchive& InOutAr, UPackageMap* InMap, bool& bOutSuccess)
{
	uint32 RepBits = 0;
	if (InOutAr.IsSaving())
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
	}

	InOutAr.SerializeBits(&RepBits, 13);

	if (RepBits & (1 << 0))
	{
		InOutAr << Instigator;
	}
	if (RepBits & (1 << 1))
	{
		InOutAr << EffectCauser;
	}
	if (RepBits & (1 << 2))
	{
		InOutAr << AbilityCDO;
	}
	if (RepBits & (1 << 3))
	{
		InOutAr << SourceObject;
	}
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(InOutAr, Actors);
	}
	if (RepBits & (1 << 5))
	{
		if (InOutAr.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());
			}
		}
		HitResult->NetSerialize(InOutAr, InMap, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		InOutAr << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}
	if (RepBits & (1 << 7))
	{
		InOutAr << bIsBlockedAttack;
	}
	if(RepBits & (1 << 8))
	{
		InOutAr << bIsCriticalAttack;
	}
	if(RepBits & (1 << 9))
	{
		InOutAr << bIsEvadedHit;
	}
	if(RepBits & (1 << 10))
	{
		InOutAr << bIsSuppressedSpell;
	}
	if(RepBits & (1 << 11))
	{
		InOutAr << bIsTargetImmune;
	}

	if (InOutAr.IsLoading())
	{
		// Just to initialize InstigatorAbilitySystemComponent
		AddInstigator(Instigator.Get(), EffectCauser.Get()); 
	}	
	
	bOutSuccess = true;
	return true;
}

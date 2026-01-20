// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ItemAffixes/ObsidianMMC_DefaultAffix.h"

#include "AbilitySystem/ObsidianAbilitySystemEffectTypes.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"

UObsidianMMC_DefaultAffix::UObsidianMMC_DefaultAffix(const FObjectInitializer& ObjectInitializer)
{
}

float UObsidianMMC_DefaultAffix::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
#if WITH_SERVER_CODE
	FObsidianGameplayEffectContext* ObsidianEffectContext = FObsidianGameplayEffectContext::ExtractEffectContextFromHandle(Spec.GetContext());
	checkf(ObsidianEffectContext, TEXT("Obsidian Gameplay Effect Context is invalid in Obsidian Damage Execution"));
	
	for (const FObsidianEffectAffixValue& AffixValue : ObsidianEffectContext->GetEffectAffixValues())
	{
		UE_LOG(LogTemp, Display, TEXT("Got affix with tag: [%s], with values:"), *AffixValue.AffixTag.ToString());
		for (const float Value : AffixValue.CurrentAffixValues)
		{
			UE_LOG(LogTemp, Display, TEXT("                                         [%f]."), Value);
			return 0;
		}
	}
#endif
	return 0;
}

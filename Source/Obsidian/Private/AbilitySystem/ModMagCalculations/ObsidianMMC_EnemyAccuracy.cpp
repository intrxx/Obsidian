// Copyright 2026 out of sCope team - intrxx

#include "AbilitySystem/ModMagCalculations/ObsidianMMC_EnemyAccuracy.h"

#include "Combat/ObsidianCombatInterface.h"
#include "Obsidian/ObsidianLogCategories.h"


float UObsidianMMC_EnemyAccuracy::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& InSpec) const
{
	IObsidianCombatInterface* CombatInterface = Cast<IObsidianCombatInterface>(InSpec.GetContext().GetSourceObject());
	if (CombatInterface == nullptr)
	{
		UE_LOG(ObLogAbilitySystem, Error, TEXT("Combat Interface on [%s] is null, please double check the Source Object"), *GetNameSafe(this));
		return Super::CalculateBaseMagnitude_Implementation(InSpec);
	}
	
	const uint8 CharacterLevel = CombatInterface->GetCharacterLevel();

	return (25.0f + (CharacterLevel * 6));
}

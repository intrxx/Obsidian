// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/ModMagCalculations/ObsidianMMC_EnemyAccuracy.h"

// ~ Core


// ~ Project
#include "Combat/ObsidianCombatInterface.h"
#include "Obsidian/ObsidianGameModule.h"

float UObsidianMMC_EnemyAccuracy::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	IObsidianCombatInterface* CombatInterface = Cast<IObsidianCombatInterface>(Spec.GetContext().GetSourceObject());
	if (CombatInterface == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("Combat Interface on [%s] is null, please double check the Source Object"), *GetNameSafe(this));
		return Super::CalculateBaseMagnitude_Implementation(Spec);
	}
	
	const int32 CharacterLevel = CombatInterface->GetCharacterLevel();

	return (25.0f + (CharacterLevel * 6));
}

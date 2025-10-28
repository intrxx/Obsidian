// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/ModMagCalculations/SpecialResource/ObsidianMMC_MaxSpecialResource.h"


#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"


float UObsidianMMC_MaxSpecialResource::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// OBS-79
	// IObsidianCombatInterface* CombatInterface = Cast<IObsidianCombatInterface>(Spec.GetContext().GetSourceObject());
	// if (CombatInterface == nullptr)
	// {
	// 	UE_LOG(LogObsidian, Error, TEXT("Combat Interface on [%s] is null, please double check the Source Object"), *GetNameSafe(this));
	// 	return Super::CalculateBaseMagnitude_Implementation(Spec);
	// }
	//
	// const int32 CharacterLevel = CombatInterface->GetCharacterLevel();
	//
	// float LevelAddedSpecialResource = 0.f;
	// if(MaxSpecialResourceAwardCurveTable)
	// {
	// 	if(const FRealCurve* Curve = MaxSpecialResourceAwardCurveTable->FindCurve(FName("MaxSpecialResource"), FString("")))
	// 	{
	// 		LevelAddedSpecialResource = Curve->Eval(CharacterLevel);
	// 	}
	// }
	//
	// return LevelAddedSpecialResource;
	return 1.0f;
}

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

UENUM(BlueprintType)
enum class EObsidianUIEffectClassification : uint8
{
	UEC_NoClassification UMETA(DisplayName = "No Classification"), 
	UEC_Buff UMETA(DisplayName = "Buff"), 
	UEC_Debuff UMETA(DisplayName = "Debuff"), 

	UEC_MAX UMETA(DisplayName = "Default MAX") 
};

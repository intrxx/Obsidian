// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

UENUM(BlueprintType)
enum class EObsidianUIEffectClassification : uint8
{
	EUEC_NoClassification UMETA(DisplayName = "No Classification"), 
	EUEC_Buff UMETA(DisplayName = "Buff"), 
	EUEC_Debuff UMETA(DisplayName = "Debuff"), 

	EUEC_MAX UMETA(DisplayName = "Default MAX") 
};

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

//TODO: Change this enum to match the coding standard (like EObsidianAbilitySpawnLocation)
UENUM(BlueprintType)
enum class EObsidianUIEffectClassification : uint8
{
	NoClassification,
	Buff,
	Debuff
};

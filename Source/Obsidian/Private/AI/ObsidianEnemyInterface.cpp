// Copyright 2024 out of sCope team - Michał Ogiński


#include "AI/ObsidianEnemyInterface.h"

// Add default functionality here for any IObsidianEnemyInterface functions that are not pure virtual.
void IObsidianEnemyInterface::SetCombatTarget_Implementation(AActor* InTarget)
{
}

AActor* IObsidianEnemyInterface::GetCombatTarget_Implementation() const
{
	return nullptr;
}

// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Data/ObsidianEnemyTypeInfo.h"

FObsidianEnemyTypeDefaultInfo UObsidianEnemyTypeInfo::GetEnemyTypeDefaultInfo(const EObsidianEnemyClass EnemyClass)
{
    return EnemyTypeDefaultInfoMap.FindChecked(EnemyClass);
}

// Copyright 2026 out of sCope team - intrxx


#include "AbilitySystem/Data/ObsidianEnemyTypeInfo.h"


FObsidianEnemyTypeDefaultInfo UObsidianEnemyTypeInfo::GetEnemyTypeDefaultInfo(const EObsidianEnemyClass InEnemyClass)
{
    return EnemyTypeDefaultInfoMap.FindChecked(InEnemyClass);
}

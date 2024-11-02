// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/ObsidianGameplayCueManager.h"

bool UObsidianGameplayCueManager::ShouldAsyncLoadRuntimeObjectLibraries() const
{
#if WITH_EDITOR
	if(GIsEditor)
	{
		return true;
	}
#endif
	return false;
}

// Copyright 2026 out of sCope team - intrxx

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

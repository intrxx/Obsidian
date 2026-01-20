// Copyright 2026 out of sCope team - intrxx

#include "AbilitySystem/ObsidianGameplayCueManager.h"

// ~ Core

// ~ Project

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

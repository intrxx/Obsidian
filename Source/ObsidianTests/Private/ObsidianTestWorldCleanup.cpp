// Copyright 2026 out of sCope team - intrxx

#include "ObsidianTestWorldCleanup.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "UObject/UObjectGlobals.h"


FObsidianTestWorldCleanup::~FObsidianTestWorldCleanup()
{
	static int32 WorldsWaitingForGarbageCollection = 0;

	if (++WorldsWaitingForGarbageCollection >= WorldsPerGarbageCollection)
	{
		WorldsWaitingForGarbageCollection = 0;
		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
	}
}

#endif // WITH_DEV_AUTOMATION_TESTS

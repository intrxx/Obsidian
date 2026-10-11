// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS

/**
 * Makes sure the worlds created by test environments do not pile up.
 *
 * Every test creates its own world, and the render scene of a world is only released when the world gets garbage
 * collected. The whole suite runs in a few seconds, so without this no garbage collection happens in between and every
 * scene stays alive. Each of them reserves gigabytes of GPU address space, after around 125 worlds the GPU runs out of
 * it and the editor crashes with DXGI_ERROR_DRIVER_INTERNAL_ERROR.
 *
 * Needs to be the first member of the test environment, declared before the FActorTestSpawner, so it gets destroyed
 * after the spawner has destroyed its world.
 */
struct FObsidianTestWorldCleanup
{
	FObsidianTestWorldCleanup() = default;
	~FObsidianTestWorldCleanup();

	FObsidianTestWorldCleanup(const FObsidianTestWorldCleanup&) = delete;
	FObsidianTestWorldCleanup& operator=(const FObsidianTestWorldCleanup&) = delete;

	/** Garbage collection takes a moment in the editor, so it only runs once for this many destroyed worlds. */
	static constexpr int32 WorldsPerGarbageCollection = 8;
};

#endif // WITH_DEV_AUTOMATION_TESTS

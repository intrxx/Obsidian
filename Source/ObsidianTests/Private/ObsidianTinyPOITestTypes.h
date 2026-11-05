// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "TinyPOITypes.h"

#include "ObsidianTinyPOITestTypes.generated.h"

/**
 * Point data used by the TinyPOI tests (see ObsidianTinyPOITests.cpp). They live in the ObsidianTests editor module, so
 * they are never part of the game, Hidden keeps them out of the editor struct pickers.
 */

USTRUCT(meta = (Hidden))
struct FObsidianTestTinyPOIRestData : public FTinyPOIPointData
{
	GENERATED_BODY()

	UPROPERTY()
	float RestDuration = 0.0f;
};

USTRUCT(meta = (Hidden))
struct FObsidianTestTinyPOIGenericData : public FTinyPOIPointData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Value = 0;
};

// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "EnvironmentQuery/EnvQueryContext.h"
#include "ObsidianEnvQueryContext_Player.generated.h"

/**
 * Made for regular enemies.
 */
UCLASS()
class OBSIDIAN_API UObsidianEnvQueryContext_Player : public UEnvQueryContext
{
	GENERATED_BODY()

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};

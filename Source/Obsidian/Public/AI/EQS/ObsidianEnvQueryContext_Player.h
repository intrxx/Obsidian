// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"

#include "ObsidianEnvQueryContext_Player.generated.h"

/**
 * Made for regular enemies.
 */
UCLASS()
class OBSIDIAN_API UObsidianEnvQueryContext_Player : public UEnvQueryContext
{
	GENERATED_BODY()

	virtual void ProvideContext(FEnvQueryInstance& InQueryInstance, FEnvQueryContextData& OutContextData) const override;
};

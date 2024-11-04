// Copyright 2024 out of sCope team - Michał Ogiński

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

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};

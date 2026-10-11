// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "MassEntityQuery.h"
#include "MassProcessor.h"

#include "ObsidianMassEnemyTargetProcessor.generated.h"

/**
 * Finds the nearest Player for every Enemy, the same way the Find Nearest Player Service of the Behavior Trees did,
 * but the Players are gathered once for all of the Enemies. Wakes up the State Trees of the Enemies that have their
 * Target close, as they only run when they are signaled.
 */
UCLASS()
class OBSIDIAN_API UObsidianMassEnemyTargetProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UObsidianMassEnemyTargetProcessor();

protected:
	//~ Begin UMassProcessor Interface
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& InEntityManager) override;
	virtual void Execute(FMassEntityManager& InEntityManager, FMassExecutionContext& InContext) override;
	//~ End UMassProcessor Interface

private:
	FMassEntityQuery EntityQuery;
};

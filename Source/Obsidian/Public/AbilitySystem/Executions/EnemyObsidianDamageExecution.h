// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "AbilitySystem/Executions/ObsidianDamageExecution.h"
#include "EnemyObsidianDamageExecution.generated.h"

/**
 * Class that adds some functionality on top of regular DamageExecution for damaging the player
 */
UCLASS()
class OBSIDIAN_API UEnemyObsidianDamageExecution : public UObsidianDamageExecution
{
	GENERATED_BODY()

public:
	UEnemyObsidianDamageExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};

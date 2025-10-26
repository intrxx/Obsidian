// Copyright 2024 out of sCope team - Michał Ogiński

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

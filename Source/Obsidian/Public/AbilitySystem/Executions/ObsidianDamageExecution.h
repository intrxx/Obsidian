// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "GameplayEffectExecutionCalculation.h"
#include "ObsidianDamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UObsidianDamageExecution(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};

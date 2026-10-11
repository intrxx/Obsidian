// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
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
	UObsidianDamageExecution(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	//~ Begin UGameplayEffectExecutionCalculation Interface
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& InExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	//~ End UGameplayEffectExecutionCalculation Interface

protected:
};

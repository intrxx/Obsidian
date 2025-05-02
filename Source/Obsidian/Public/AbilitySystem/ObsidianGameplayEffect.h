// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "ObsidianGameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UObsidianGameplayEffect(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};

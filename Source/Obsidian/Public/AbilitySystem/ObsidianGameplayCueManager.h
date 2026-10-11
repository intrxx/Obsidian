// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"

#include "ObsidianGameplayCueManager.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()

protected:
	//~ Begin UGameplayCueManager Interface
	virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const override;
	//~ End UGameplayCueManager Interface
};

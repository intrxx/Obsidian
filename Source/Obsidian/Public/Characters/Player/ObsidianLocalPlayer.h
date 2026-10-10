// Copyright 2024 Michał Ogiński

#pragma once

#include "CommonLocalPlayer.h"
#include "CoreMinimal.h"

#include "ObsidianLocalPlayer.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianLocalPlayer : public UCommonLocalPlayer
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;
};

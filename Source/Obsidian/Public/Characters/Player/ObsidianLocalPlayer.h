// Copyright 2024 Michał Ogiński

#pragma once

#include <CoreMinimal.h>


#include <CommonLocalPlayer.h>
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

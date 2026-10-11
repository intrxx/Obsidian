// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "AIController.h"
#include "CoreMinimal.h"

#include "AObsidianAIControllerBase.generated.h"

/**
 * Base class of the AI Controllers, the brain (Behavior Tree, State Tree) is added by the child classes.
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	AObsidianAIControllerBase();
};

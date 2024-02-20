// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Characters/ObsidianCharacterBase.h"
#include "Interaction/ObsidianHighlightInterface.h"
#include "ObsidianEnemy.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianEnemy : public AObsidianCharacterBase, public IObsidianHighlightInterface
{
	GENERATED_BODY()

public:
	AObsidianEnemy();
	
	virtual void StartHighlight() override;
	virtual void StopHighlight() override;
};

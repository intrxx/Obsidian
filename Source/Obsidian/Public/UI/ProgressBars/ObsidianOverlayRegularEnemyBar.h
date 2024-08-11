// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ProgressBars/ObsidianBasicHealthBar.h"
#include "ObsidianOverlayRegularEnemyBar.generated.h"

class UObsidianEnemyAttributesComponent;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianOverlayRegularEnemyBar : public UObsidianBasicHealthBar
{
	GENERATED_BODY()

protected:
	virtual void HandleWidgetControllerSet() override;
	void SetInitialValues(const UObsidianEnemyAttributesComponent* EnemyAttributesComp) const;
	
};

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ProgressBars/ObsidianBasicHealthBar.h"
#include "ObsidianOverlayRegularEnemyBar.generated.h"

class UCommonTextBlock;
class UObsidianEnemyAttributesComponent;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianOverlayRegularEnemyBar : public UObsidianBasicHealthBar
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> EnemyName_TextBlock;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UProgressBar> StaggerMeter_ProgressBar;

	float StaggerMeter = 0.0f;
	float MaxStaggerMeter = 0.0f;
	
protected:
	virtual void HandleWidgetControllerSet() override;
	
	void SetInitialValues(const UObsidianEnemyAttributesComponent* EnemyAttributesComp);
};
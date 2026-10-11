// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "UI/ProgressBars/ObsidianProgressBarBase.h"

#include "ObsidianHeroStaggerBar.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianHeroStaggerBar : public UObsidianProgressBarBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UProgressBar> Stagger_ProgressBar;

protected:
	//~ Begin UObsidianWidgetBase Interface
	virtual void HandleWidgetControllerSet() override;
	//~ End UObsidianWidgetBase Interface

	float Stagger = 0.f;
	float MaxStagger = 0.f;

private:
	UFUNCTION()
	void StaggerChanged(const float InNewStagger);
	UFUNCTION()
	void MaxStaggerChanged(const float InNewMaxStagger);
};

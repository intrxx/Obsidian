// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


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
	float Stagger = 0.f;
	float MaxStagger = 0.f;
	
protected:
	virtual void HandleWidgetControllerSet() override;

private:
	UFUNCTION()
	void StaggerChanged(const float NewStagger);
	UFUNCTION()
	void MaxStaggerChanged(const float NewMaxStagger);
};

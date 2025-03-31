// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ProgressBars/ObsidianProgressBarBase.h"
#include "ObsidianOverlayExperienceBar.generated.h"

class USizeBox;
class UProgressBar;
class UOverlay;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianOverlayExperienceBar : public UObsidianProgressBarBase
{
	GENERATED_BODY()

protected:
	virtual void HandleWidgetControllerSet() override;
	
	void ExperienceChanged(const float NewValue);
	void MaxExperienceChanged(const float NewValue, const float OldValue);
	
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UOverlay> Main_Overlay;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UProgressBar> Experience_ProgressBar;
	
private:
	float Experience = 0.f;
	float MaxExperience = 0.f;
	float LastMaxExperience = 0.f;
};

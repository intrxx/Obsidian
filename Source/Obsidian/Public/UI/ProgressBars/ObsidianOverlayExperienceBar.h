// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UI/ProgressBars/ObsidianProgressBarBase.h"
#include "ObsidianOverlayExperienceBar.generated.h"

class UObMainOverlayWidgetController;
class UObsidianOverlayExperienceInfo;
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
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	void ExperienceChanged(const float NewValue);
	void MaxExperienceChanged(const float NewValue, const float OldValue);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TSubclassOf<UObsidianOverlayExperienceInfo> ExperienceInfoWidgetClass;

	UPROPERTY()
	TObjectPtr<UObsidianOverlayExperienceInfo> ExperienceInfo;
	
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UOverlay> Main_Overlay;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UProgressBar> Experience_ProgressBar;
	
private:
	UPROPERTY()
	TObjectPtr<UObMainOverlayWidgetController> MainOverlayWidgetController;
	
	float Experience = 0.f;
	float MaxExperience = 0.f;
	float LastMaxExperience = 0.f;
};

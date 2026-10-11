// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

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
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API UObsidianOverlayExperienceBar : public UObsidianProgressBarBase
{
	GENERATED_BODY()

protected:
	void ExperienceChanged(const float InNewValue);
	void MaxExperienceChanged(const float InNewValue, const float InOldValue);

	//~ Begin UObsidianWidgetBase Interface
	virtual void HandleWidgetControllerSet() override;
	//~ End UObsidianWidgetBase Interface

	//~ Begin UUserWidget Interface
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	//~ End UUserWidget Interface

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

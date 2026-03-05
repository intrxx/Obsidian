// Copyright 2026 out of sCope - intrxx

#pragma once

#include <CoreMinimal.h>


#include "UI/ProgressBars/ObsidianProgressBarBase.h"
#include "ObsidianOverlayStaminaBar.generated.h"

class UObMainOverlayWidgetController;
class USizeBox;
class UOverlay;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianOverlayStaminaBar : public UObsidianProgressBarBase
{
	GENERATED_BODY()

protected:
	virtual void HandleWidgetControllerSet() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	UFUNCTION()
	void StaminaChanged(const float NewValue);
	UFUNCTION()
	void MaxStaminaChanged(const float NewValue);

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UOverlay> Main_Overlay;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UProgressBar> Stamina_ProgressBar;
	
private:
	UPROPERTY()
	TObjectPtr<UObMainOverlayWidgetController> MainOverlayWidgetController;
	
	float Stamina = 0.f;
	float MaxStamina = 0.f;
};

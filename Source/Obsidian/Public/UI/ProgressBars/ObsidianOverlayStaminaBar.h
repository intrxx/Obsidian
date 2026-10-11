// Copyright 2026 out of sCope - intrxx

#pragma once

#include "CoreMinimal.h"

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
	UFUNCTION()
	void StaminaChanged(const float InNewValue);
	UFUNCTION()
	void MaxStaminaChanged(const float InNewValue);

	//~ Begin UObsidianWidgetBase Interface
	virtual void HandleWidgetControllerSet() override;
	//~ End UObsidianWidgetBase Interface

	//~ Begin UUserWidget Interface
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//~ End UUserWidget Interface

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

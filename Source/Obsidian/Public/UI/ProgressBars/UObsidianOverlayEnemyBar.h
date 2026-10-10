// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "UI/ProgressBars/ObsidianBasicHealthBar.h"

#include "UObsidianOverlayEnemyBar.generated.h"

class UImage;
class UObsidianEnemyOverlayBarComponent;
class UCommonTextBlock;
class UObsidianEnemyAttributesComponent;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianOverlayEnemyBar : public UObsidianBasicHealthBar
{
	GENERATED_BODY()

protected:
	void ResetStyle() const;
	void ResetSpecialStyle() const;
	void SetOverlayBarStyle(const FSlateBrush& InBrush) const;
	void HandleSpecialEffectApplied(const FSlateBrush& InBrush) const;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void HandleWidgetControllerSet() override;
	
	void SetInitialValues();

	void HealthChanged(const float InNewValue);
	void MaxHealthChanged(const float InNewValue);
	void EnergyShieldChanged(const float InNewValue);
	void MaxEnergyShieldChanged(const float InNewValue);
	void StaggerMeterChanged(const float InNewValue);
	void MaxStaggerMeterChanged(const float InNewValue);

protected:
	UPROPERTY()
	TObjectPtr<UObsidianEnemyOverlayBarComponent> EnemyOverlayBarComp;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> EnemyName_TextBlock;

	/* Image to display special effects like immunity or temporary damage reduction. **/
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> SpecialEffect_Image;

	/** Basic fill image cached for convenience. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian")
	FSlateBrush OverlayBarFillImage;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UProgressBar> StaggerMeter_ProgressBar;
	
	float StaggerMeter = 0.0f;
	float MaxStaggerMeter = 0.0f;
	
private:
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle EnergyShieldChangedDelegateHandle;
	FDelegateHandle MaxEnergyShieldChangedDelegateHandle;
	FDelegateHandle StaggerMeterChangedDelegateHandle;
	FDelegateHandle MaxStaggerMeterChangedDelegateHandle;
};

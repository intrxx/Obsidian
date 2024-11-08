// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianTypes/ObsidianUITypes.h"
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
	void SetOverlayBarStyle(const FSlateBrush& Brush) const;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void HandleWidgetControllerSet() override;
	
	void SetInitialValues();

	void HealthChanged(const float NewValue);
	void MaxHealthChanged(const float NewValue);
	void EnergyShieldChanged(const float NewValue);
	void MaxEnergyShieldChanged(const float NewValue);
	void StaggerMeterChanged(const float NewValue);
	void MaxStaggerMeterChanged(const float NewValue);

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

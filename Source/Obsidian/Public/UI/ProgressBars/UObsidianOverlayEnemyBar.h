// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianTypes/ObsidianUITypes.h"
#include "UI/ProgressBars/ObsidianBasicHealthBar.h"
#include "UObsidianOverlayEnemyBar.generated.h"

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
	virtual void HandleWidgetControllerSet() override;
	
	void SetInitialValues();

	UFUNCTION()
	void HealthChanged(const float NewValue);
	UFUNCTION()
	void MaxHealthChanged(const float NewValue);
	UFUNCTION()
	void EnergyShieldChanged(const float NewValue);
	UFUNCTION()
	void MaxEnergyShieldChanged(const float NewValue);
	UFUNCTION()
	void StaggerMeterChanged(const float NewValue);
	UFUNCTION()
	void MaxStaggerMeterChanged(const float NewValue);

protected:
	UPROPERTY()
	TObjectPtr<UObsidianEnemyOverlayBarComponent> EnemyOverlayBarComp;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> EnemyName_TextBlock;

	/** Basic fill image cached for convenience. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian")
	FSlateBrush OverlayBarFillImage;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UProgressBar> StaggerMeter_ProgressBar;
	
	float StaggerMeter = 0.0f;
	float MaxStaggerMeter = 0.0f;
	
private:
	bool bIsBind = false;
	
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle EnergyShieldChangedDelegateHandle;
	FDelegateHandle MaxEnergyShieldChangedDelegateHandle;
	FDelegateHandle StaggerMeterChangedDelegateHandle;
	FDelegateHandle MaxStaggerMeterChangedDelegateHandle;
};

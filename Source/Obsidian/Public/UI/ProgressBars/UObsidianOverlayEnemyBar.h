// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianTypes/ObsidianUITypes.h"
#include "UI/ProgressBars/ObsidianBasicHealthBar.h"
#include "UObsidianOverlayEnemyBar.generated.h"

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
	UPROPERTY()
	TObjectPtr<UObsidianEnemyAttributesComponent> EnemyAttributesComp;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> EnemyName_TextBlock;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UProgressBar> StaggerMeter_ProgressBar;

	float StaggerMeter = 0.0f;
	float MaxStaggerMeter = 0.0f;
	
protected:
	virtual void HandleWidgetControllerSet() override;
	virtual void UninitAndDestroy();
	
	void SetInitialValues();
	
	void HealthChanged(const float NewValue);
	void MaxHealthChanged(const float NewValue);
	void EnergyShieldChanged(const float NewValue);
	void MaxEnergyShieldChanged(const float NewValue);
	void StaggerMeterChanged(const float NewValue);
	void MaxStaggerMeterChanged(const float NewValue);

	UFUNCTION()
	void HandleUIData(const FObsidianEffectUIDataWidgetRow Row);
	UFUNCTION()
	void HandleStackingUIData(const FObsidianEffectUIDataWidgetRow Row, const FObsidianEffectUIStackingData StackingData);
	
private:
	bool bIsBind = false;
	
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle EnergyShieldChangedDelegateHandle;
	FDelegateHandle MaxEnergyShieldChangedDelegateHandle;
	FDelegateHandle StaggerMeterChangedDelegateHandle;
	FDelegateHandle MaxStaggerMeterChangedDelegateHandle;
};

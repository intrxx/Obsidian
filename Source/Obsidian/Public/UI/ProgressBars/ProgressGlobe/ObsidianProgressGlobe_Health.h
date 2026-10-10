// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobeBase.h"

#include "ObsidianProgressGlobe_Health.generated.h"

class UObsidianRadialProgressBar;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianProgressGlobe_Health : public UObsidianProgressGlobe
{
	GENERATED_BODY()	

public:
	void SetProgressGlobeStyle(const FSlateBrush& InProgressGlobeFillImage) const;
	void ResetStyle() const;
	
protected:
	virtual void HandleWidgetControllerSet() override;

	UFUNCTION()
	void OnHealthChanged(float InNewHealth);
	UFUNCTION()
	void OnMaxHealthChanged(float InNewMaxHealth);
	UFUNCTION()
	void OnEnergyShieldChanged(float InNewEnergyShield);
	UFUNCTION()
	void OnMaxEnergyShieldChanged(float InNewMaxEnergyShield);
	UFUNCTION()
	void OnStaggerMeterChanged(float InNewStaggerMeter);
	UFUNCTION()
	void OnMaxStaggerMeterChanged(float InNewMaxStaggerMeter);

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HealthProgressGlobe", meta=(BindWidget))
	TObjectPtr<UHorizontalBox> EnergyShieldAttribute_HorizontalBox;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HealthProgressGlobe", meta=(BindWidget))
	TObjectPtr<UProgressBar> Health_ProgressGlobe;
	
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HealthProgressGlobe", meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> HealthAttributeName_TextBlock;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HealthProgressGlobe", meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> HealthAttributeCount_TextBlock;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HealthProgressGlobe", meta=(BindWidget))
	TObjectPtr<UProgressBar> EnergyShield_ProgressGlobe;
	
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HealthProgressGlobe", meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> EnergyShieldAttributeName_TextBlock;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HealthProgressGlobe", meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> EnergyShieldAttributeCount_TextBlock;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HealthProgressGlobe", meta=(BindWidget))
	TObjectPtr<UObsidianRadialProgressBar> Stagger_RadialProgressBar;
	
private:
	float Health = 0.0f;
	float MaxHealth = 0.0f;
	float EnergyShield = 0.0f;
	float MaxEnergyShield = 0.0f;
	float StaggerMeter = 0.0f;
	float MaxStaggerMeter = 0.0f;
};

// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "UI/ProgressBars/ObsidianBasicHealthBar.h"

#include "ObsidianHeroHealthBar_Simple.generated.h"

/**
 * Health bar that should be displayed on simulated proxies.
 */
UCLASS()
class OBSIDIAN_API UObsidianHeroHealthBar_Simple : public UObsidianBasicHealthBar
{
	GENERATED_BODY()

protected:
	//~ Begin UObsidianWidgetBase Interface
	virtual void HandleWidgetControllerSet() override;
	//~ End UObsidianWidgetBase Interface

private:
	UFUNCTION()
	void HealthChanged(const float InNewHealth);
	UFUNCTION()
	void MaxHealthChanged(const float InNewMaxHealth);

	UFUNCTION()
	void EnergyShieldChanged(const float InNewEnergyShield);
	UFUNCTION()
	void MaxEnergyShieldChanged(const float InNewMaxEnergyShield);
};

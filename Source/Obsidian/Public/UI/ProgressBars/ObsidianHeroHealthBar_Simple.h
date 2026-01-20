// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


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
	virtual void HandleWidgetControllerSet() override;

private:
	UFUNCTION()
	void HealthChanged(const float NewHealth);
	UFUNCTION()
	void MaxHealthChanged(const float NewMaxHealth);
    
	UFUNCTION()
	void EnergyShieldChanged(const float NewEnergyShield);
	UFUNCTION()
	void MaxEnergyShieldChanged(const float NewMaxEnergyShield);
	
};

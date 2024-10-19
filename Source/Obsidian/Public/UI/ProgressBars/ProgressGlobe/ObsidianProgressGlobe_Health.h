// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobeBase.h"
#include "ObsidianProgressGlobe_Health.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianProgressGlobe_Health : public UObsidianProgressGlobe
{
	GENERATED_BODY()

protected:
	virtual void HandleWidgetControllerSet() override;

	UFUNCTION()
	void OnHealthChanged(float NewHealth);
	UFUNCTION()
	void OnMaxHealthChanged(float NewMaxHealth);
	UFUNCTION()
	void OnEnergyShieldChanged(float NewEnergyShield);
	UFUNCTION()
	void OnMaxEnergyShieldChanged(float NewMaxEnergyShield);

private:
	float Health = 0.0f;
	float MaxHealth = 0.0f;
	float EnergyShield = 0.0f;
	float MaxEnergyShield = 0.0f;
};

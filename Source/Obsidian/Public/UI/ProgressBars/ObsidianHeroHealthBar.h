// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "UI/ProgressBars/ObsidianBasicHealthBar.h"

#include "ObsidianHeroHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianHeroHealthBar : public UObsidianBasicHealthBar
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
    TObjectPtr<UProgressBar> Mana_ProgressBar;

protected:
    virtual void HandleWidgetControllerSet() override;
    
protected:
    float Mana = 0.f;
    float MaxMana = 0.f;

private:
    UFUNCTION()
    void HealthChanged(const float InNewHealth);
    UFUNCTION()
    void MaxHealthChanged(const float InNewMaxHealth);
    
    UFUNCTION()
    void EnergyShieldChanged(const float InNewEnergyShield);
    UFUNCTION()
    void MaxEnergyShieldChanged(const float InNewMaxEnergyShield);

    UFUNCTION()
    void ManaChanged(const float InNewMana);
    UFUNCTION()
    void MaxManaChanged(const float InNewMaxMana);
};

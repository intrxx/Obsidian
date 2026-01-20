// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


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
    void HealthChanged(const float NewHealth);
    UFUNCTION()
    void MaxHealthChanged(const float NewMaxHealth);
    
    UFUNCTION()
    void EnergyShieldChanged(const float NewEnergyShield);
    UFUNCTION()
    void MaxEnergyShieldChanged(const float NewMaxEnergyShield);

    UFUNCTION()
    void ManaChanged(const float NewMana);
    UFUNCTION()
    void MaxManaChanged(const float NewMaxMana);
};

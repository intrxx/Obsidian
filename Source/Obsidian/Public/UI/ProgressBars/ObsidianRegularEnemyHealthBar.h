// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ProgressBars/ObsidianBasicHealthBar.h"
#include "ObsidianRegularEnemyHealthBar.generated.h"

class UObsidianEnemyAttributesComponent;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianRegularEnemyHealthBar : public UObsidianBasicHealthBar
{
	GENERATED_BODY()

protected:
    virtual void HandleWidgetControllerSet() override;

    void HealthChanged(const float NewValue);
    void MaxHealthChanged(const float NewValue);
    void EnergyShieldChanged(const float NewValue);
    void MaxEnergyShieldChanged(const float NewValue);
    
private:
    void StartWidgetHideTimer();
    void HideWidget();

private:
    /** Time after which this health bar will become hidden */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = true), Category = "Obsidian")
    float HideTime = 10.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = true), Category = "Obsidian")
    bool bShouldHideWidget = true;
    
    FTimerHandle HideTimerHandle;
    
};

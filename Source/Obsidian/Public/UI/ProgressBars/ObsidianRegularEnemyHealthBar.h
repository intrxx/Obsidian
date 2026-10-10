// Copyright 2026 out of sCope team - intrxx

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

    void HealthChanged(const float InNewValue);
    void MaxHealthChanged(const float InNewValue);
    void EnergyShieldChanged(const float InNewValue);
    void MaxEnergyShieldChanged(const float InNewValue);
    
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

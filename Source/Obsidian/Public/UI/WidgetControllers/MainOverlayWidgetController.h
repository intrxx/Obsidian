// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetControllerBase.h"
#include "GameplayTagContainer.h"
#include "ObsidianTypes/ObsidianUITypes.h"
#include "MainOverlayWidgetController.generated.h"

struct FObsidianEffectUIData;
class UOStackingDurationalEffectInfo;
class UObsidianDurationalEffectInfo;
class UObsidianAbilitySystemComponent;
class UObsidianEffectInfoBase;

/** Delegate used for notifying Progress Globes to display the healing/replenish amount */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEffectUIGlobeData, const float, EffectDuration, const float, EffectMagnitude);

/** Delegate used for updating the target for health bar displayed on player's hud */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateEnemyTargetForHealthBar, AActor*, TargetActor, const bool, bDisplayHealthBar);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAuraWidgetDestructionInfoReceived, const FGameplayTag, WidgetTag);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class OBSIDIAN_API UMainOverlayWidgetController : public UObsidianHeroWidgetControllerBase
{
	GENERATED_BODY()

public:
	// ~ Start of UObsidianWidgetController
	virtual void OnWidgetControllerSetupCompleted() override;
	// ~ End of UObsidianWidgetController

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "Obsidian|Health")
	void UpdateHealthInfoGlobe(const float Magnitude) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "Obsidian|Mana")
	void UpdateManaInfoGlobe(const float Magnitude) const;

	virtual void SetInitialAttributeValues() const override;
	virtual void SetInitialStaggerMeter() const;
	virtual void SetInitialExperienceValues() const;

public:
	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|Mana")
	FOnAttributeValueChangedSignature OnManaChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|Mana")
	FOnAttributeValueChangedSignature OnMaxManaChangedDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|SpecialResource")
	FOnAttributeValueChangedSignature OnSpecialResourceChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|SpecialResource")
	FOnAttributeValueChangedSignature OnMaxSpecialResourceChangedDelegate;
	
	FOnAttributeValueChangedOneParam OnExperienceChangedDelegate;
	FOnAttributeValueChangedTwoParams OnMaxExperienceChangedDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|Health")
	FOnAttributeValueChangedSignature OnHealthChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|Health")
	FOnAttributeValueChangedSignature OnMaxHealthChangedDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|EnergyShield")
	FOnAttributeValueChangedSignature OnEnergyShieldChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|EnergyShield")
	FOnAttributeValueChangedSignature OnMaxEnergyShieldChangedDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|StaggerMeter")
	FOnAttributeValueChangedSignature OnStaggerMeterChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|StaggerMeter")
	FOnAttributeValueChangedSignature OnMaxStaggerMeterChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|UIData")
	FEffectUIDataWidgetRow EffectUIDataWidgetRowDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|UIData")
	FEffectUIGlobeData EffectUIHealthGlobeDataDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|UIData")
	FEffectUIGlobeData EffectUIManaGlobeDataDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|UIData")
	FStackingEffectUIDataWidgetRow EffectStackingUIDataDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|UIData")
	FOnUpdateEnemyTargetForHealthBar OnUpdateRegularEnemyTargetForHealthBarDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|UIData")
	FOnUpdateEnemyTargetForHealthBar OnUpdateBossEnemyTargetForHealthBarDelegate;

	FOnAuraWidgetDestructionInfoReceived OnAuraWidgetDestructionInfoReceivedDelegate;

protected:
	virtual void HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC) override;
	
	void HandleEffectApplied(const FObsidianEffectUIData& UIData);

	void ManaChanged(const FOnAttributeChangeData& Data) const;
	void MaxManaChanged(const FOnAttributeChangeData& Data) const;
	void SpecialResourceChanged(const FOnAttributeChangeData& Data) const;
	void MaxSpecialResourceChanged(const FOnAttributeChangeData& Data) const;
	void ExperienceChanged(const FOnAttributeChangeData& Data) const;
	void MaxExperienceChanged(const FOnAttributeChangeData& Data);
	
	void HealthChanged(const FOnAttributeChangeData& Data) const;
	void MaxHealthChanged(const FOnAttributeChangeData& Data) const;
	void EnergyShieldChanged(const FOnAttributeChangeData& Data) const;
	void MaxEnergyShieldChanged(const FOnAttributeChangeData& Data) const;
	void StaggerMeterChanged(const FOnAttributeChangeData& Data) const;
	void MaxStaggerMeterChanged(const FOnAttributeChangeData& Data) const;

	UFUNCTION()
	void UpdateHoveringOverTarget(AActor* TargetActor, const bool bHoveredOver);
	UFUNCTION()
	void UpdateBossDetectionInfo(AActor* BossActor, const bool bSeen);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|UIData")
	TObjectPtr<UDataTable> UIEffectDataWidgetTable;
	
	/** Hero Set */
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;
	FDelegateHandle SpecialResourceChangedDelegateHandle;
	FDelegateHandle MaxSpecialResourceChangedDelegateHandle;
	FDelegateHandle ExperienceChangedDelegateHandle;
	FDelegateHandle MaxExperienceChangedDelegateHandle;

	/** Common Set */
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle EnergyShieldChangedDelegateHandle;
	FDelegateHandle MaxEnergyShieldChangedDelegateHandle;
	FDelegateHandle StaggerMeterChangedDelegateHandle;
	FDelegateHandle MaxStaggerMeterChangedDelegateHandle;

private:
	UFUNCTION()
	void DestroyAuraWidget(const FGameplayTag AuraWidgetTag);

private:
	float MaxExperienceOldValue = 0.0f;
};

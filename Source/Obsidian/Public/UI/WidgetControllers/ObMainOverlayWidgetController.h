// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "ObsidianTypes/ObsidianUITypes.h"
#include "UI/ObsidianWidgetControllerBase.h"

#include "ObMainOverlayWidgetController.generated.h"

struct FObsidianEffectUIData;

class UOStackingDurationalEffectInfo;
class UObsidianDurationalEffectInfo;
class UObsidianAbilitySystemComponent;
class UObsidianHeroAttributesComponent;
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
class OBSIDIAN_API UObMainOverlayWidgetController : public UObsidianHeroWidgetControllerBase
{
	GENERATED_BODY()

public:
	// ~ Start of UObsidianWidgetController
	virtual void OnWidgetControllerSetupCompleted() override;
	// ~ End of UObsidianWidgetController

	FObsidianSpecialResourceVisuals GetSpecialResourceVisuals() const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "Obsidian|Health")
	void UpdateHealthInfoGlobe(const float InMagnitude) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "Obsidian|Mana")
	void UpdateManaInfoGlobe(const float InMagnitude) const;

	void SetInitialAttributeValues() const;
	void SetInitialStaggerMeter() const;
	void SetInitialExperienceValues();
	void SetInitialStaminaValues();

public:
	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|Mana")
	FOnAttributeValueChangedSignature OnManaChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|Mana")
	FOnAttributeValueChangedSignature OnMaxManaChangedDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|SpecialResource")
	FOnAttributeValueChangedSignature OnSpecialResourceChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|SpecialResource")
	FOnAttributeValueChangedSignature OnMaxSpecialResourceChangedDelegate;

	FOnAttributeValueChangedSignature OnStaminaChangedDelegate;
	FOnAttributeValueChangedSignature OnMaxStaminaChangedDelegate;
	
	FOnAttributeValueChangedOneParam OnExperienceChangedDelegate;
	FOnAttributeValueChangedTwoParams OnMaxExperienceChangedDelegate;
	
	FOnAttributeValueChangedSignature OnPassiveSkillPointsChangedDelegate;
	FOnAttributeValueChangedSignature OnAscensionPointsChangedDelegate;
	
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
	virtual void HandleBindingCallbacks(UObsidianAbilitySystemComponent* InObsidianASC) override;
	
	void HandleEffectApplied(const FObsidianEffectUIData& InUIData);

	void ManaChanged(const FOnAttributeChangeData& InData) const;
	void MaxManaChanged(const FOnAttributeChangeData& InData) const;
	void SpecialResourceChanged(const FOnAttributeChangeData& InData) const;
	void MaxSpecialResourceChanged(const FOnAttributeChangeData& InData) const;
	void ExperienceChanged(const FOnAttributeChangeData& InData) const;
	void MaxExperienceChanged(const FOnAttributeChangeData& InData);
	void PassiveSkillPointsChanged(const FOnAttributeChangeData& InData) const;
	void AscensionPointsChanged(const FOnAttributeChangeData& InData) const;
	void StaminaChanged(const FOnAttributeChangeData& InData) const;
	void MaxStaminaChanged(const FOnAttributeChangeData& InData) const;
	
	void HealthChanged(const FOnAttributeChangeData& InData) const;
	void MaxHealthChanged(const FOnAttributeChangeData& InData) const;
	void EnergyShieldChanged(const FOnAttributeChangeData& InData) const;
	void MaxEnergyShieldChanged(const FOnAttributeChangeData& InData) const;
	void StaggerMeterChanged(const FOnAttributeChangeData& InData) const;
	void MaxStaggerMeterChanged(const FOnAttributeChangeData& InData) const;

	UFUNCTION()
	void UpdateHoveringOverTarget(AActor* InTargetActor, const bool bInHoveredOver);
	UFUNCTION()
	void UpdateBossDetectionInfo(AActor* InBossActor, const bool bInSeen);
	
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
	FDelegateHandle PassiveSkillPointsChangedDelegateHandle;
	FDelegateHandle AscensionPointsChangedDelegateHandle;
	FDelegateHandle OnStaminaChangedDelegateHandle;
	FDelegateHandle OnMaxStaminaChangedDelegateHandle;

	/** Common Set */
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle EnergyShieldChangedDelegateHandle;
	FDelegateHandle MaxEnergyShieldChangedDelegateHandle;
	FDelegateHandle StaggerMeterChangedDelegateHandle;
	FDelegateHandle MaxStaggerMeterChangedDelegateHandle;

private:
	UFUNCTION()
	void DestroyAuraWidget(const FGameplayTag InAuraWidgetTag);

private:
	UPROPERTY()
	TWeakObjectPtr<UObsidianAbilitySystemComponent> OwnerAbilitySystemComponent = nullptr;
	UPROPERTY()
	TWeakObjectPtr<UObsidianHeroAttributesComponent> OwnerAttributesComponent = nullptr;
	
 	float MaxExperienceOldValue = 0.0f;
};

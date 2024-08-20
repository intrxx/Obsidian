// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetController.h"
#include "GameplayTagContainer.h"
#include "ObsidianTypes/UserIterface/ObsidianUIEffectClassification.h"
#include "ObsidianTypes/UserIterface/ObsidianUIData.h"
#include "MainOverlayWidgetController.generated.h"

class UOStackingDurationalEffectInfo;
class UObsidianDurationalEffectInfo;
class UObsidianAbilitySystemComponent;
class UObsidianEffectInfoBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeValueChangedSignature, float, NewValue);

UENUM(BlueprintType)
enum class EObsidianInfoWidgetType : uint8
{
	EIWT_SimpleEffectInfo UMETA(DisplayName = "Simple Effect Info"),
	EIWT_DurationalEffectInfo UMETA(DisplayName = "Durational Effect Info"),
	EIWT_StackingEffectInfo UMETA(DisplayName = "Stacking Effect Info"),
	EIWT_StackingDurationalEffectInfo UMETA(DisplayName = "Stacking Durational Effect Info"),

	EIWT_MAX UMETA(DisplayName = "Default MAX") 
};

USTRUCT(BlueprintType)
struct FObsidianEffectUIDataWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EObsidianInfoWidgetType InfoWidgetType = EObsidianInfoWidgetType::EIWT_SimpleEffectInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EffectTag = FGameplayTag();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EObsidianUIEffectClassification EffectClassification = EObsidianUIEffectClassification::EUEC_NoClassification;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText EffectName = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText EffectDesc = FText();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> EffectImage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="InfoWidgetType == EObsidianInfoWidgetType::EIWT_SimpleEffectInfo", EditConditionHides))
	TSubclassOf<UObsidianEffectInfoBase> SimpleEffectWidget;

	/** Indicates that this effect is an Aura Gameplay Ability that will be cleared manually */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="InfoWidgetType == EObsidianInfoWidgetType::EIWT_SimpleEffectInfo", EditConditionHides))
	bool bAuraEffect = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="InfoWidgetType == EObsidianInfoWidgetType::EIWT_DurationalEffectInfo", EditConditionHides))
	TSubclassOf<UObsidianDurationalEffectInfo> DurationalEffectWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="InfoWidgetType == EObsidianInfoWidgetType::EIWT_StackingEffectInfo", EditConditionHides))
	TSubclassOf<UObsidianEffectInfoBase> StackingEffectWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="InfoWidgetType == EObsidianInfoWidgetType::EIWT_StackingDurationalEffectInfo", EditConditionHides))
	TSubclassOf<UOStackingDurationalEffectInfo> StackingDurationalEffectWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float EffectDuration = 0.f;
};

/** Broadcasts DataTable Row that corresponds to a given asset tag */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEffectUIDataWidgetRow, FObsidianEffectUIDataWidgetRow, Row);

/** Broadcasts DataTable Row that corresponds to a given asset tag as well as the stacking information */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStackingEffectUIDataWidgetRow, FObsidianEffectUIDataWidgetRow, Row, FObsidianEffectUIStackingData, StackingData);

/** Delegate used for notifying Progress Globes to display the healing/replenish amount */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEffectUIGlobeData, const float&, EffectDuration, const float&, EffectMagnitude);

/** Delegate used for updating the target for health bar displayed on player's hud */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateEnemyTargetForHealthBar, AActor*, TargetActor, const bool, bDisplayHealthBar);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAuraWidgetDestructionInfoReceived, const FGameplayTag, WidgetTag);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class OBSIDIAN_API UMainOverlayWidgetController : public UObsidianWidgetController
{
	GENERATED_BODY()

public:
	// ~ Start of UObsidianWidgetController
	virtual void OnWidgetControllerSetupCompleted() override;
	// ~ End of UObsidianWidgetController

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "Obsidian|Health")
	void UpdateHealthInfoGlobe(const float& Magnitude) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "Obsidian|Mana")
	void UpdateManaInfoGlobe(const float& Magnitude) const;

	virtual void SetInitialAttributeValues() const override;

public:
	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|Health")
	FOnAttributeValueChangedSignature OnHealthChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|Health")
	FOnAttributeValueChangedSignature OnMaxHealthChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|Mana")
	FOnAttributeValueChangedSignature OnManaChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|Mana")
	FOnAttributeValueChangedSignature OnMaxManaChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|EnergyShield")
	FOnAttributeValueChangedSignature OnEnergyShieldChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|EnergyShield")
	FOnAttributeValueChangedSignature OnMaxEnergyShieldChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|SpecialResource")
	FOnAttributeValueChangedSignature OnSpecialResourceChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|SpecialResource")
	FOnAttributeValueChangedSignature OnMaxSpecialResourceChangedDelegate;

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
	
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);

	void HealthChanged(const FOnAttributeChangeData& Data) const;
	void MaxHealthChanged(const FOnAttributeChangeData& Data) const;
	void EnergyShieldChanged(const FOnAttributeChangeData& Data) const;
	void MaxEnergyShieldChanged(const FOnAttributeChangeData& Data) const;
	void ManaChanged(const FOnAttributeChangeData& Data) const;
	void MaxManaChanged(const FOnAttributeChangeData& Data) const;
	void SpecialResourceChanged(const FOnAttributeChangeData& Data) const;
	void MaxSpecialResourceChanged(const FOnAttributeChangeData& Data) const;
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
};

template <typename T>
T* UMainOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}

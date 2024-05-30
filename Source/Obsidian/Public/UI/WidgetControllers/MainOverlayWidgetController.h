// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetController.h"
#include "GameplayTagContainer.h"
#include "ObsidianTypes/ObsidianUIEffectClassification.h"
#include "ObsidianTypes/ObsidianUIData.h"
#include "MainOverlayWidgetController.generated.h"

class UOStackingDurationalEffectInfo;
class UObsidianDurationalEffectInfo;
class UObsidianAbilitySystemComponent;
class UObsidianEffectInfoBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeValueChangedSignature, float, NewValue);

UENUM(BlueprintType)
enum class EObsidianInfoWidgetType : uint8
{
	IWT_SimpleEffectInfo,
	IWT_DurationalEffectInfo,
	IWT_StackingEffectInfo,
	IWT_StackingDurationalEffectInfo
};

USTRUCT(BlueprintType)
struct FObsidianEffectUIDataWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EObsidianInfoWidgetType InfoWidgetType = EObsidianInfoWidgetType::IWT_SimpleEffectInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EffectTag = FGameplayTag();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EObsidianUIEffectClassification EffectClassification = EObsidianUIEffectClassification::UEC_NoClassification;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText EffectName = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText EffectDesc = FText();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> EffectImage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="InfoWidgetType == EObsidianInfoWidgetType::SimpleEffectInfo", EditConditionHides))
	TSubclassOf<UObsidianEffectInfoBase> SimpleEffectWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="InfoWidgetType == EObsidianInfoWidgetType::DurationalEffectInfo", EditConditionHides))
	TSubclassOf<UObsidianDurationalEffectInfo> DurationalEffectWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="InfoWidgetType == EObsidianInfoWidgetType::StackingEffectInfo", EditConditionHides))
	TSubclassOf<UObsidianEffectInfoBase> StackingEffectWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="InfoWidgetType == EObsidianInfoWidgetType::StackingDurationalEffectInfo", EditConditionHides))
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

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|Attributes|MaxEnergyShield")
	FOnAttributeValueChangedSignature OnMaxEnergyShieldChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|UIData")
	FEffectUIDataWidgetRow EffectUIDataWidgetRowDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|UIData")
	FEffectUIGlobeData EffectUIHealthGlobeDataDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|UIData")
	FEffectUIGlobeData EffectUIManaGlobeDataDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|UIData")
	FStackingEffectUIDataWidgetRow EffectStackingUIDataDelegate;

protected:
	virtual void HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC) override;
	virtual void SetInitialAttributeValues() const override;
	
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);

	void HealthChanged(const FOnAttributeChangeData& Data) const;
	void MaxHealthChanged(const FOnAttributeChangeData& Data) const;
	void EnergyShieldChanged(const FOnAttributeChangeData& Data) const;
	void MaxEnergyShieldChanged(const FOnAttributeChangeData& Data) const;
	void ManaChanged(const FOnAttributeChangeData& Data) const;
	void MaxManaChanged(const FOnAttributeChangeData& Data) const;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|UIData")
	TObjectPtr<UDataTable> UIEffectDataWidgetTable;
	
	/** Hero Set */
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;

	/** Common Set */
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle EnergyShieldChangedDelegateHandle;
	FDelegateHandle MaxEnergyShieldChangedDelegateHandle;
};

template <typename T>
T* UMainOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetController.h"
#include "GameplayTagContainer.h"
#include "ObsidianTypes/ObsidianUIEffectClassification.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "MainOverlayWidgetController.generated.h"

class UOStackingDurationalEffectInfo;
class UObsidianDurationalEffectInfo;
class UObsidianEffectInfoBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeValueChangedSignature, float, NewValue);

UENUM(BlueprintType)
enum class EObsidianInfoWidgetType : uint8
{
	SimpleEffectInfo,
	DurationalEffectInfo,
	StackingEffectInfo,
	StackingDurationalEffectInfo
};

USTRUCT(BlueprintType)
struct FObsidianEffectUIDataWidgetRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	void SetEffectDuration(const float Duration) {EffectDuration = Duration;};
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EObsidianInfoWidgetType InfoWidgetType = EObsidianInfoWidgetType::SimpleEffectInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EffectTag = FGameplayTag();

	//TODO Delete this
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsEffectDurational = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EObsidianUIEffectClassification EffectClassification = EObsidianUIEffectClassification::NoClassification;
	
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

private:
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
	virtual void BroadcastControllerToAttributesComp(UObsidianAttributesComponent* AC) override;
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
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|UIData")
	TObjectPtr<UDataTable> UIEffectDataWidgetTable;
};

template <typename T>
T* UMainOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}

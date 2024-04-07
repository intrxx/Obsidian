// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetController.h"
#include "GameplayTagContainer.h"
#include "MainOverlayWidgetController.generated.h"

class UObsidianEffectInfoBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeValueChangedSignature, float, NewValue);

USTRUCT(BlueprintType)
struct FObsidianEffectUIDataWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EffectTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsEffectDurational = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText EffectName = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText EffectDesc = FText();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> EffectImage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UObsidianEffectInfoBase> EffectWidget;
};

// Broadcasts DataTable Row that corresponds to a given asset tag as well as it's duration
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEffectUIDataWidgetRow, FObsidianEffectUIDataWidgetRow, Row, const float&, EffectDuration);

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

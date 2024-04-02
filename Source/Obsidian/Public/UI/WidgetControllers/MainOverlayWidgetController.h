// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetController.h"
#include "MainOverlayWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChanged, float, NewMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaChanged, float, NewMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxManaChanged, float, NewMaxMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnergyShieldChanged, float, NewEnergyShield);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxEnergyShieldChanged, float, MaxNewEnergyShield);

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

	UPROPERTY(BlueprintAssignable, Category = "UI|Attributes|Health")
	FOnHealthChanged OnHealthChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "UI|Attributes|Health")
	FOnMaxHealthChanged OnMaxHealthChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "UI|Attributes|Mana")
	FOnManaChanged OnManaChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "UI|Attributes|Mana")
	FOnMaxManaChanged OnMaxManaChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "UI|Attributes|EnergyShield")
	FOnEnergyShieldChanged OnEnergyShieldChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "UI|Attributes|MaxEnergyShield")
	FOnMaxEnergyShieldChanged OnMaxEnergyShieldChangedDelegate;
};

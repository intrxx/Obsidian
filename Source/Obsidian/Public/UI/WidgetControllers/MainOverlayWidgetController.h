// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetController.h"
#include "MainOverlayWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChanged, float, NewMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaChanged, float, NewMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxManaChanged, float, NewMaxMana);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class OBSIDIAN_API UMainOverlayWidgetController : public UObsidianWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastControllerToAttributesComp(UObsidianAttributesComponent* AC) override;

	UPROPERTY(BlueprintAssignable, Category = "UI|Attributes")
	FOnHealthChanged OnHealthChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "UI|Attributes")
	FOnMaxHealthChanged OnMaxHealthChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "UI|Attributes")
	FOnManaChanged OnManaChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "UI|Attributes")
	FOnMaxManaChanged OnMaxManaChangedDelegate;
};

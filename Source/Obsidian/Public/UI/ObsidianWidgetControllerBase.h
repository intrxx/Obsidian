// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems/ObsidianInventoryComponent.h"
#include "ObsidianWidgetControllerBase.generated.h"

class UObsidianInventoryComponent;
class UObsidianHeroAttributesComponent;
class UObsidianAbilitySystemComponent;
class UObsidianAttributesComponent;
class APlayerState;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FObsidianWidgetControllerParams
{
	GENERATED_BODY()
	
	FObsidianWidgetControllerParams(APlayerController* PC = nullptr, APlayerState* PS = nullptr, UAbilitySystemComponent* ASC = nullptr,
		UObsidianHeroAttributesComponent* AC = nullptr, UObsidianInventoryComponent* IC = nullptr)
	: PlayerController(PC)
	, PlayerState(PS)
	, AbilitySystemComponent(ASC)
	, AttributesComponent(AC)
	, InventoryComponent(IC)
	{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UObsidianHeroAttributesComponent> AttributesComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UObsidianInventoryComponent> InventoryComponent = nullptr;
};

/**
 * 
 */
UCLASS(Abstract)
class OBSIDIAN_API UObsidianHeroWidgetControllerBase : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|WidgetController")
	void SetWidgetControllerParams(const FObsidianWidgetControllerParams& WidgetControllerParams);
	
	/** This function is called when the initial setup for Widget Controller is completed, widget controller contains valid data */
	virtual void OnWidgetControllerSetupCompleted();

	virtual void SetInitialAttributeValues() const;

protected:
	virtual void HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|HeroWidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|HeroWidgetController")
	TObjectPtr<APlayerState> PlayerState;
	
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|HeroWidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HeroWidgetController")
	TObjectPtr<UObsidianHeroAttributesComponent> AttributesComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HeroWidgetController")
	TObjectPtr<UObsidianInventoryComponent> InventoryComponent;
};


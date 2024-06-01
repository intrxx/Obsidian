// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ObsidianWidgetController.generated.h"

class UObsidianHeroAttributesComponent;
class UObsidianAbilitySystemComponent;
class UObsidianAttributesComponent;
class APlayerState;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams() {}

	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UObsidianHeroAttributesComponent* AC)
	: PlayerController(PC)
	, PlayerState(PS)
	, AbilitySystemComponent(ASC)
	, AttributesComponent(AC)
	{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UObsidianHeroAttributesComponent> AttributesComponent = nullptr;
};

/**
 * 
 */
UCLASS(Abstract)
class OBSIDIAN_API UObsidianWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|WidgetController")
	void SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams);
	
	/** This function is called when the initial setup for Widget Controller is completed, widget controller contains valid data */
	virtual void OnWidgetControllerSetupCompleted();

	virtual void SetInitialAttributeValues() const;

protected:
	virtual void HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|WidgetController")
	TObjectPtr<APlayerState> PlayerState;
	
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|WidgetController")
	TObjectPtr<UObsidianHeroAttributesComponent> AttributesComponent;
};


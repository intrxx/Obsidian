// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianWidgetController.generated.h"

class UObsidianAttributesComponent;
class APlayerState;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams() {}

	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UObsidianAttributesComponent* AC)
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
	TObjectPtr<UObsidianAttributesComponent> AttributesComponent = nullptr;
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|WidgetController")
	void SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|WidgetController")
	TObjectPtr<APlayerState> PlayerState;
	
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|WidgetController")
	TObjectPtr<UObsidianAttributesComponent> AttributeComponent;
	
};

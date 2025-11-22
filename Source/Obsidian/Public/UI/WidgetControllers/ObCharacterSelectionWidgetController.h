// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include "UI/ObsidianWidgetControllerBase.h"
#include "ObCharacterSelectionWidgetController.generated.h"

class AObsidianCharacterCreationHero;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObCharacterSelectionWidgetController : public UObsidianHeroWidgetControllerBase
{
	GENERATED_BODY()

public:
	// ~ Start of WidgetControllerBase
	virtual void OnWidgetControllerSetupCompleted() override;
	// ~ End of WidgetControllerBase

	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterCreation")
	void HighlightCharacterWithTag(const EObsidianHeroClass WithClass);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterCreation")
	void ResetHighlightForCharacterWithTag(const EObsidianHeroClass WithClass);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterCreation")
	AObsidianCharacterCreationHero* GetCreationHeroForTag(const EObsidianHeroClass ForClass);

	void SetupCameraForCreationPanel();
	void ResetCameraToDefaultViewTarget();

private:
	void GatherCreationHeroes();
	void GatherViewTargets();
	
private:
	UPROPERTY()
	TArray<TObjectPtr<AObsidianCharacterCreationHero>> CreationHeroes;

	UPROPERTY()
	TObjectPtr<AActor> MainMenuDefaultViewTarget = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> CharacterCreationViewTarget = nullptr;
};


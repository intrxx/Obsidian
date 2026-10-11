// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

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
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterCreation")
	void HighlightCharacterWithTag(const EObsidianHeroClass InWithClass);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterCreation")
	void ResetHighlightForCharacterWithTag(const EObsidianHeroClass InWithClass);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterCreation")
	AObsidianCharacterCreationHero* GetCreationHeroForTag(const EObsidianHeroClass InForClass);

	UFUNCTION(BlueprintCallable, Category = "Obisidan")
	void SetupCameraForCreationPanel();
	UFUNCTION(BlueprintCallable, Category = "Obisidan")
	void ResetCameraToDefaultViewTarget();

	//~ Begin UObsidianHeroWidgetControllerBase Interface
	virtual void OnWidgetControllerSetupCompleted() override;
	//~ End UObsidianHeroWidgetControllerBase Interface

private:
	void GatherCreationHeroes();
	void GatherViewTargets();

	UPROPERTY()
	TArray<TObjectPtr<AObsidianCharacterCreationHero>> CreationHeroes;

	UPROPERTY()
	TObjectPtr<AActor> MainMenuDefaultViewTarget = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> CharacterCreationViewTarget = nullptr;
};


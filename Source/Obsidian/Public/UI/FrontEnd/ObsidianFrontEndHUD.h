// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include <GameFramework/HUD.h>
#include "ObsidianFrontEndHUD.generated.h"

struct FObsidianWidgetControllerParams;

class UObsidianLocalPlayer;
class AObsidianPlayerController;
class UObCharacterSelectionWidgetController;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianFrontEndHUD : public AHUD
{
	GENERATED_BODY()

public:
	AObsidianFrontEndHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UObCharacterSelectionWidgetController* GetCharacterSelectionWidgetController(const FObsidianWidgetControllerParams& WidgetControllerParams);

protected:
	/**
	 * Widget Controllers 
	 */

	UPROPERTY()
	TObjectPtr<UObCharacterSelectionWidgetController> CharacterSelectionWidgetController;

	UPROPERTY(EditAnywhere, Category = "ObsidianUI|MainOverlay")
	TSubclassOf<UObCharacterSelectionWidgetController> CharacterSelectionWidgetControllerClass;
};

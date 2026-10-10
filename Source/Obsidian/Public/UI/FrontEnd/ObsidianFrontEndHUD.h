// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

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
	AObsidianFrontEndHUD(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	UObCharacterSelectionWidgetController* GetCharacterSelectionWidgetController(
		const FObsidianWidgetControllerParams& InWidgetControllerParams);

protected:
	/**
	 * Widget Controllers 
	 */

	UPROPERTY()
	TObjectPtr<UObCharacterSelectionWidgetController> CharacterSelectionWidgetController;

	UPROPERTY(EditAnywhere, Category = "ObsidianUI|MainOverlay")
	TSubclassOf<UObCharacterSelectionWidgetController> CharacterSelectionWidgetControllerClass;
};

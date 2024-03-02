// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ObsidianHUD.generated.h"

class UObsidianAttributesComponent;
class UAbilitySystemComponent;
struct FWidgetControllerParams;
class UMainOverlayWidgetController;
class UObsidianMainOverlay;
class UObsidianWidgetBase;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UMainOverlayWidgetController* GetMainOverlayWidgetController(const FWidgetControllerParams& WidgetControllerParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UObsidianAttributesComponent* AC);
	
public:
	UPROPERTY()
	TObjectPtr<UObsidianWidgetBase> OverlayWidget;

private:
	UPROPERTY(EditAnywhere, Category = "ObsidianUI|MainOverlay")
	TSubclassOf<UObsidianMainOverlay> MainOverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UMainOverlayWidgetController> MainOverlayWidgetController;

	UPROPERTY(EditAnywhere, Category = "ObsidianUI|MainOverlay")
	TSubclassOf<UMainOverlayWidgetController> MainOverlayWidgetControllerClass;
};

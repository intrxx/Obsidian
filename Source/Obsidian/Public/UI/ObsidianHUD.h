// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianWidgetControllerBase.h"
#include "GameFramework/HUD.h"
#include "ObsidianHUD.generated.h"

struct FObsidianHeroWidgetControllerParams;
class UOCharacterStatusWidgetController;
class UObsidianHeroAttributesComponent;
class UAbilitySystemComponent;
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
	UMainOverlayWidgetController* GetMainOverlayWidgetController(const FObsidianWidgetControllerParams& WidgetControllerParams);
	UOCharacterStatusWidgetController* GetCharacterStatusWidgetController(const FObsidianWidgetControllerParams& WidgetControllerParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UObsidianHeroAttributesComponent* AC);

	void ToggleCharacterStatus() const;
	void ToggleInventory() const;
	
public:
	UPROPERTY()
	TObjectPtr<UObsidianMainOverlay> MainOverlayWidget;
	
private:
	UPROPERTY(EditAnywhere, Category = "ObsidianUI|MainOverlay")
	TSubclassOf<UObsidianMainOverlay> MainOverlayWidgetClass;

	/**
	 * Widget Controllers 
	 */
	
	UPROPERTY()
	TObjectPtr<UMainOverlayWidgetController> MainOverlayWidgetController;

	UPROPERTY(EditAnywhere, Category = "ObsidianUI|MainOverlay")
	TSubclassOf<UMainOverlayWidgetController> MainOverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UOCharacterStatusWidgetController> CharacterStatusWidgetController;

	UPROPERTY(EditAnywhere, Category = "ObsidianUI|CharacterStatus")
	TSubclassOf<UOCharacterStatusWidgetController> CharacterStatusWidgetControllerClass;
};

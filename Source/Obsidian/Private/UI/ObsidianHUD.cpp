// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ObsidianHUD.h"
#include "UI/MainOverlay/ObsidianMainOverlay.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"

UMainOverlayWidgetController* AObsidianHUD::GetMainOverlayWidgetController(const FWidgetControllerParams& WidgetControllerParams)
{
	// If the main controller is a nullptr we need to construct one
	if(MainOverlayWidgetController == nullptr)
	{
		if(ensureMsgf(MainOverlayWidgetControllerClass, TEXT("Main Overlay Widget Controller Class is not set on HUD Class [%s], please fill it out in BP_ObsidianHUD"), *GetNameSafe(this)))
		{
			MainOverlayWidgetController = NewObject<UMainOverlayWidgetController>(this, MainOverlayWidgetControllerClass);
			MainOverlayWidgetController->SetWidgetControllerParams(WidgetControllerParams);
			
			return MainOverlayWidgetController;
		}
	}
	return MainOverlayWidgetController;
}

void AObsidianHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UObsidianHeroAttributesComponent* AC)
{
	if(ensureMsgf(MainOverlayWidgetClass, TEXT("Main Overlay Widget Class is not set on HUD Class [%s], please fill it out in BP_ObsidianHUD"), *GetNameSafe(this)))
	{
		MainOverlayWidget = CreateWidget<UObsidianMainOverlay>(GetWorld(), MainOverlayWidgetClass);

		const FWidgetControllerParams Params(PC, PS, ASC, AC);
		UMainOverlayWidgetController* WidgetController = GetMainOverlayWidgetController(Params);

		MainOverlayWidget->SetWidgetController(WidgetController);
		WidgetController->OnWidgetControllerSetupCompleted();
		
		MainOverlayWidget->AddToViewport();
	}
}

void AObsidianHUD::ToggleCharacterStatus() const
{
	if(MainOverlayWidget)
	{
		MainOverlayWidget->ToggleCharacterStatus();
	}
}

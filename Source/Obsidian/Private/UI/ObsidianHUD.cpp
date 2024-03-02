// Copyright 2024 Michał Ogiński


#include "UI/ObsidianHUD.h"
#include "UI/ObsidianMainOverlay.h"
#include "UI/WidgetControllers/MainOverlayWidgetController.h"

UMainOverlayWidgetController* AObsidianHUD::GetMainOverlayWidgetController(const FWidgetControllerParams& WidgetControllerParams)
{
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

void AObsidianHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UObsidianAttributesComponent* AC)
{
	if(ensureMsgf(MainOverlayWidgetClass, TEXT("Main Overlay Widget Class is not set on HUD Class [%s], please fill it out in BP_ObsidianHUD"), *GetNameSafe(this)))
	{
		OverlayWidget = CreateWidget<UObsidianWidgetBase>(GetWorld(), MainOverlayWidgetClass);

		const FWidgetControllerParams Params(PC, PS, ASC, AC);
		UMainOverlayWidgetController* WidgetController = GetMainOverlayWidgetController(Params);

		OverlayWidget->SetWidgetController(WidgetController);
		OverlayWidget->AddToViewport();
	}
}

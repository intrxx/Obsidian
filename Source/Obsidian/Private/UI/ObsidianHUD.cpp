// Copyright 2024 Michał Ogiński


#include "UI/ObsidianHUD.h"
#include "UI/ObsidianMainOverlay.h"

void AObsidianHUD::BeginPlay()
{
	Super::BeginPlay();

	if(MainOverlayWidgetClass)
	{
		OverlayWidget = CreateWidget<UObsidianWidgetBase>(GetWorld(), MainOverlayWidgetClass);
		OverlayWidget->AddToViewport();
	}
}

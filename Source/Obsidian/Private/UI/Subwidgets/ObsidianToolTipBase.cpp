// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Subwidgets/ObsidianToolTipBase.h"

// ~ Core
#include "Blueprint/WidgetLayoutLibrary.h"

// ~ Project

FVector2D UObsidianToolTipBase::GetDesiredViewportPosition() const
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return FVector2D::Zero();
	}
	
	const FVector2D MouseViewportPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(World);
	return MouseViewportPosition + ToolTipOffset;
}

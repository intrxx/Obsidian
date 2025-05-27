// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Subwidgets/ObsidianToolTipBase.h"

// ~ Core
#include "Blueprint/WidgetLayoutLibrary.h"

// ~ Project

FVector2D UObsidianToolTipBase::GetDesiredPosition() const
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return FVector2D::Zero();
	}
	
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(World);
	return MousePosition + ToolTipOffset;
}

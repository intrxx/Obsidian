// Copyright 2024 Michał Ogiński


#include "UI/OverlaysSubwidgets/ObsidianEffectInfoBase.h"
#include "UI/OverlaysSubwidgets/ObsidianEffectDescription.h"


void UObsidianEffectInfoBase::OnEffectHovered()
{
	if(EffectDescWidgetClass == nullptr)
	{
		return;
	}

	EffectDescWidget = CreateWidget<UObsidianEffectDescription>(this, EffectDescWidgetClass);
	EffectDescWidget->InitEffectDesc(EffectName, EffectDesc);
	
	const FVector2D DescPos = EffectDescWidget->GetDescriptionDesiredPosition();
	EffectDescWidget->SetPositionInViewport(DescPos);
	EffectDescWidget->AddToViewport();
}

void UObsidianEffectInfoBase::OnEffectUnHovered()
{
	if(EffectDescWidget)
	{
		EffectDescWidget->RemoveFromParent();
	}
}


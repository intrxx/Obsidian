// Copyright 2026 out of sCope team - intrxx

#include "UI/MainOverlay/Subwidgets/ObsidianEffectInfoBase.h"

// ~ Core
#include "Components/Button.h"

// ~ Project
#include "UI/MainOverlay/Subwidgets/ObsidianEffectDescription.h"

void UObsidianEffectInfoBase::NativeConstruct()
{
	Super::NativeConstruct();

	Interact_Button->OnHovered.AddDynamic(this, &ThisClass::OnEffectHovered);
	Interact_Button->OnUnhovered.AddDynamic(this, &ThisClass::OnEffectUnHovered);
}

void UObsidianEffectInfoBase::NativeDestruct()
{
	if(Interact_Button)
	{
		if(Interact_Button->OnHovered.IsBound())
		{
			Interact_Button->OnHovered.Clear();
		}
		if(Interact_Button->OnUnhovered.IsBound())
		{
			Interact_Button->OnUnhovered.Clear();
		}
	}
	
	Super::NativeDestruct();
}

void UObsidianEffectInfoBase::RemoveAuraInfoWidget()
{
	OnEffectUnHovered();
	RemoveFromParent();
}

void UObsidianEffectInfoBase::OnEffectHovered()
{
	checkf(EffectDescWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianEffectInfoBase::OnEffectHovered, fill it in ObsidianEffectInfo"));
	EffectDescWidget = CreateWidget<UObsidianEffectDescription>(this, EffectDescWidgetClass);
	EffectDescWidget->InitEffectDesc(EffectName, EffectDesc);
	
	const FVector2D DescViewportPosition = EffectDescWidget->GetDesiredViewportPosition();
	EffectDescWidget->SetPositionInViewport(DescViewportPosition);
	EffectDescWidget->AddToViewport();
}

void UObsidianEffectInfoBase::OnEffectUnHovered()
{
	if(EffectDescWidget)
	{
		EffectDescWidget->RemoveFromParent();
	}
}



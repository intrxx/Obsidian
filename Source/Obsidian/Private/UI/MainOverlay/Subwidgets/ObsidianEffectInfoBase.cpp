// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/MainOverlay/Subwidgets/ObsidianEffectInfoBase.h"

#include "Components/Button.h"
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

void UObsidianEffectInfoBase::OnEffectHovered()
{
	if(EffectDescWidgetClass == nullptr)
	{
		return;
	}

	EffectDescWidget = CreateWidget<UObsidianEffectDescription>(this, EffectDescWidgetClass);
	EffectDescWidget->InitEffectDesc(EffectName, EffectDesc);
	
	const FVector2D DescPosition = EffectDescWidget->GetDescriptionDesiredPosition();
	EffectDescWidget->SetPositionInViewport(DescPosition);
	EffectDescWidget->AddToViewport();
}

void UObsidianEffectInfoBase::OnEffectUnHovered()
{
	if(EffectDescWidget)
	{
		EffectDescWidget->RemoveFromParent();
	}
}

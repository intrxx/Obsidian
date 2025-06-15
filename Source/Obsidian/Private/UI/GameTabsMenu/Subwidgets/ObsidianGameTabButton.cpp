// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/GameTabsMenu/Subwidgets/ObsidianGameTabButton.h"

// ~ Core
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

// ~ Project
#include "UI/GameTabsMenu/Subwidgets/ObsidianGameTabTooltip.h"

void UObsidianGameTabButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	Root_SizeBox->SetWidthOverride(SizeBoxWidth);
	Root_SizeBox->SetHeightOverride(SizeBoxHeight);

	Border_Image->SetBrush(BorderBrush);
	
	Tab_Button->SetStyle(TabButtonStyle);
}

void UObsidianGameTabButton::NativeConstruct()
{
	Super::NativeConstruct();
	
	Tab_Button->OnHovered.AddDynamic(this, &ThisClass::OnTabButtonHovered);
	Tab_Button->OnUnhovered.AddDynamic(this, &ThisClass::OnTabButtonUnhovered);
	Tab_Button->OnClicked.AddDynamic(this, &ThisClass::OnTabButtonUnhovered);
}

void UObsidianGameTabButton::OnTabButtonHovered()
{
	if(bIsCorrespondingTabOpen)
	{
		return;
	}

	checkf(TabToolTipWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianGameTabButton::OnTabButtonHovered, fill it in UObsidianGameTabButton instance."));
	TabToolTipWidget = CreateWidget<UObsidianGameTabTooltip>(this, TabToolTipWidgetClass);
	
	const FVector2D DesiredViewportPosition = TabToolTipWidget->GetDesiredViewportPosition();
	
	TabToolTipWidget->SetPositionInViewport(DesiredViewportPosition);
	TabToolTipWidget->AddToViewport();
}

void UObsidianGameTabButton::OnTabButtonUnhovered()
{
	if(TabToolTipWidget && TabToolTipWidget->IsConstructed())
	{
		TabToolTipWidget->RemoveFromParent();
	}
}

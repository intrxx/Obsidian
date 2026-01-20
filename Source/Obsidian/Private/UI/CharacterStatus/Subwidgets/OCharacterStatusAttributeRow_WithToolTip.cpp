// Copyright 2026 out of sCope team - intrxx

#include "UI/CharacterStatus/Subwidgets/OCharacterStatusAttributeRow_WithToolTip.h"

// ~ Core
#include "Components/Button.h"

// ~ Project
#include "UI/Subwidgets/ObsidianToolTipBase.h"
#include "UI/CharacterStatus/ObsidianCharacterStatus.h"
#include "UI/CharacterStatus/Subwidgets/ObsidianAttributeToolTip.h"

void UOCharacterStatusAttributeRow_WithToolTip::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(bToolTipEnabled)
	{
		OnToolTipButtonHovered();
	}
}

void UOCharacterStatusAttributeRow_WithToolTip::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if(bToolTipEnabled)
	{
		OnToolTipButtonUnHovered();
	}
}

void UOCharacterStatusAttributeRow_WithToolTip::SetCharacterStatus(UObsidianCharacterStatus* InCharacterStatus)
{
	CharacterStatus = InCharacterStatus;
	
	CharacterStatus->OnWidgetDestroyedDelegate.AddUObject(this, &ThisClass::HandleCharacterStatusSet);
}

void UOCharacterStatusAttributeRow_WithToolTip::OnToolTipButtonHovered()
{
	if(!AttributeToolTipWidget)
	{
		checkf(AttributeToolTipWidgetClass, TEXT("Tried to create widget without valid widget class in OCharacterStatusAttributeRow_WithToolTip"));
		AttributeToolTipWidget = CreateWidget<UObsidianAttributeToolTip>(this, AttributeToolTipWidgetClass);
		AttributeToolTipWidget->InitAttributeToolTip(AttributeDescription);

		const FVector2D ViewportPosition = AttributeToolTipWidget->GetDesiredViewportPosition();
		AttributeToolTipWidget->SetPositionInViewport(ViewportPosition);
		AttributeToolTipWidget->AddToViewport();
	}
}

void UOCharacterStatusAttributeRow_WithToolTip::OnToolTipButtonUnHovered()
{
	if(AttributeToolTipWidget && AttributeToolTipWidget->IsConstructed())
	{
		AttributeToolTipWidget->RemoveFromParent();
		AttributeToolTipWidget = nullptr;
	}
}

void UOCharacterStatusAttributeRow_WithToolTip::HandleCharacterStatusSet()
{
	if(AttributeToolTipWidget && AttributeToolTipWidget->IsConstructed())
	{
		AttributeToolTipWidget->RemoveFromParent();
	}
}

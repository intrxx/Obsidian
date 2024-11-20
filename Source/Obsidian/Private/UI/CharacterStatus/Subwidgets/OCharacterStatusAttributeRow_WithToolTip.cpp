// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/CharacterStatus/Subwidgets/OCharacterStatusAttributeRow_WithToolTip.h"
#include "UI/Subwidgets/ObsidianToolTipBase.h"
#include "Components/Button.h"
#include "UI/CharacterStatus/ObsidianCharacterStatus.h"
#include "UI/CharacterStatus/Subwidgets/ObsidianAttributeToolTip.h"

void UOCharacterStatusAttributeRow_WithToolTip::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(!bToolTipButtonEnabled)
	{
		/**
		* This does not actually disable the button, only makes it un-testable for hits
		* The reason is that there is a bug or design flaw in Unreal Engine that automatically greys out every child of disabled buttons.
		*/
		
		//Tooltip_Button->SetIsEnabled(false);
		Tooltip_Button->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		//Tooltip_Button->SetIsEnabled(true);
		Tooltip_Button->SetVisibility(ESlateVisibility::Visible);
	}
}

void UOCharacterStatusAttributeRow_WithToolTip::NativeConstruct()
{
	Super::NativeConstruct();

	if(Tooltip_Button)
	{
		Tooltip_Button->OnHovered.AddDynamic(this, &ThisClass::OnToolTipButtonHovered);
		Tooltip_Button->OnUnhovered.AddDynamic(this, &ThisClass::OnToolTipButtonUnHovered);
	}
}

void UOCharacterStatusAttributeRow_WithToolTip::BeginDestroy()
{
	if(Tooltip_Button)
	{
		Tooltip_Button->OnHovered.Clear();
		Tooltip_Button->OnUnhovered.Clear();
	}
	
	Super::BeginDestroy();
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

		const FVector2D Position = AttributeToolTipWidget->GetDesiredPosition();
		AttributeToolTipWidget->SetPositionInViewport(Position);
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

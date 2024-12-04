// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianGroundItemDesc.h"
#include "Components/Image.h"


void UObsidianGroundItemDesc::NativeConstruct()
{
	Super::NativeConstruct();

	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(RegularBackgroundColor);		
	}
}

FReply UObsidianGroundItemDesc::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//TODO Maybe some widget animation
	UE_LOG(LogTemp, Warning, TEXT("YO mouse down"));
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnItemDescMouseButtonDownDelegate.Broadcast();

		//const TSharedRef<SWidget> SlateWidget = TakeWidget();
		//return FReply::Handled()
		//	.DetectDrag(SlateWidget, EKeys::LeftMouseButton)
		//	.CaptureMouse(SlateWidget)
		//	.SetUserFocus(SlateWidget, EFocusCause::Mouse);
	}

	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
}

void UObsidianGroundItemDesc::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(HoveredBackgroundColor);
	}
	OnItemDescMouseHoverDelegate.Broadcast(true);
	
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UObsidianGroundItemDesc::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(RegularBackgroundColor);
	}
	OnItemDescMouseHoverDelegate.Broadcast(false);
	
	Super::NativeOnMouseLeave(InMouseEvent);
}

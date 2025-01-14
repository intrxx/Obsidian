// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/SubWidgets/ObsidianUnstackSlider.h"
#include "CommonTextBlock.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/Slider.h"

void UObsidianUnstackSlider::NativeConstruct()
{
	Super::NativeConstruct();

	Close_Button->OnClicked.AddDynamic(this, &ThisClass::OnCloseButtonClicked);
	Accept_Button->OnClicked.AddDynamic(this, &ThisClass::OnAcceptButtonClicked);
	Stacks_Slider->OnValueChanged.AddDynamic(this, &ThisClass::UpdateStacksValues);
}

void UObsidianUnstackSlider::NativeDestruct()
{
	Close_Button->OnClicked.Clear();
	Accept_Button->OnClicked.Clear();
	Stacks_Slider->OnValueChanged.Clear();
	
	Super::NativeDestruct();
}

void UObsidianUnstackSlider::InitializeUnstackSlider(const int32 CurrentItemStacks)
{
	MaxStacks = CurrentItemStacks;
	StackToLeave = MaxStacks - 1;
	StackToTake = 1;
	
	const FText CurrentItemStacksText = FText::FromString(FString::Printf(TEXT("%d"), StackToLeave));
	StacksToLeave_TextBlock->SetText(CurrentItemStacksText);
	StacksToTake_TextBlock->SetText(FText::FromString(TEXT("1")));

	Stacks_Slider->SetMaxValue(MaxStacks);
	Stacks_Slider->SetValue(1);
}

FVector2D UObsidianUnstackSlider::GetSizeBoxSize() const
{
	return FVector2D(Root_SizeBox->GetWidthOverride(), Root_SizeBox->GetHeightOverride());
}

float UObsidianUnstackSlider::GetTopDesiredOffset() const
{
	return TopDesiredOffset;
}

void UObsidianUnstackSlider::DestroyUnstackSlider()
{
	OnAcceptButtonPressedDelegate.Clear();
	RemoveFromParent();
}

void UObsidianUnstackSlider::OnCloseButtonClicked()
{
	RemoveFromParent();
}

void UObsidianUnstackSlider::OnAcceptButtonClicked()
{
	OnAcceptButtonPressedDelegate.Broadcast(StackToTake);
	RemoveFromParent();
}

void UObsidianUnstackSlider::UpdateStacksValues(float NewValue)
{
	const int32 IntValue = FMath::RoundToInt32(NewValue);
	StackToTake = IntValue;
	StackToLeave = MaxStacks - StackToTake;

	const FText StacksToTakeText = FText::FromString(FString::Printf(TEXT("%d"), StackToTake));
	StacksToTake_TextBlock->SetText(StacksToTakeText);
	
	const FText StacksToLeaveText = FText::FromString(FString::Printf(TEXT("%d"), StackToLeave));
	StacksToLeave_TextBlock->SetText(StacksToLeaveText);
}

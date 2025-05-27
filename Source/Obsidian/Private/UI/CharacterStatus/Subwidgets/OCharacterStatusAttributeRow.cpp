// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/CharacterStatus/Subwidgets/OCharacterStatusAttributeRow.h"

// ~ Core
#include "CommonTextBlock.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"

// ~ Project

void UOCharacterStatusAttributeRow::NativePreConstruct()
{
	Super::NativePreConstruct();

	InitialSetup();
}

void UOCharacterStatusAttributeRow::InitialSetup() const
{
	Root_SizeBox->SetWidthOverride(SizeBoxWidth);
	Root_SizeBox->SetHeightOverride(SizeBoxHeight);
	AttributeName_TextBlock->SetStyle(AttributeNameStyle);
	AttributeName_TextBlock->SetText(AttributeName);
	AttributeValue_TextBlock->SetStyle(AttributeValueStyle);
	NameAndValue_Spacer->SetSize(FVector2D(NameAndValueSpacing, 1.f));
}

void UOCharacterStatusAttributeRow::SetAttributeValue(const float Value) const
{
	const FText TextValue = FText::FromString(FString::Printf(TEXT("%d"), FMath::FloorToInt(Value)));
	if(AttributeValue_TextBlock)
	{
		AttributeValue_TextBlock->SetText(TextValue);
	}
}

void UOCharacterStatusAttributeRow::SetAttributeValueWithPercentage(const float Value) const
{
	const FText TextValue = FText::FromString(FString::Printf(TEXT("%d%%"), FMath::FloorToInt(Value)));
	if(AttributeValue_TextBlock)
	{
		AttributeValue_TextBlock->SetText(TextValue);
	}
}

void UOCharacterStatusAttributeRow::SetTwoAttributeValuesWithPercent(const float Value, const float MaxValue) const
{
	const FText TextValue = FText::FromString(FString::Printf(TEXT("%d%% (%d%%)"),
		FMath::FloorToInt(Value), FMath::FloorToInt(MaxValue)));
	
	if(AttributeValue_TextBlock)
	{
		AttributeValue_TextBlock->SetText(TextValue);
	}
}


// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/SubWidgets/ObsidianSimpleStackableItemDesc.h"
#include "CommonTextBlock.h"

void UObsidianSimpleStackableItemDesc::SetStackCount(const int32 CurrentStacks, const int32 MaxStacks)
{
	const FText StackCountText = FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentStacks, MaxStacks));
	StackCount_TextBlock->SetText(StackCountText);
}

void UObsidianSimpleStackableItemDesc::SetItemDescription(const FText& ItemDescription)
{
	if(!ItemDescription.IsEmpty())
	{
		ItemDescription_TextBlock->SetText(ItemDescription);
	}
}

void UObsidianSimpleStackableItemDesc::SetAdditionalItemDescription(const FText& AdditionalItemDescription)
{
	if(!AdditionalItemDescription.IsEmpty())
	{
		AdditionalItemDescription_TextBlock->SetText(AdditionalItemDescription);
	}
}

// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianItemDescriptionBase.h"

#include "CommonTextBlock.h"

void UObsidianItemDescriptionBase::SetItemDisplayName(const FText& DisplayName)
{
	if(!DisplayName.IsEmpty())
	{
		ItemName_TextBlock->SetText(DisplayName);
	}
}

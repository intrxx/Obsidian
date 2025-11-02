// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/Items/ObsidianAffixRow.h"

#include <CommonTextBlock.h>

void UObsidianAffixRow::NativePreConstruct()
{
	Super::NativePreConstruct();

	RefreshAffixRowStyle();
}

void UObsidianAffixRow::RefreshAffixRowStyle()
{
	if (AffixText_TextBlock)
	{
		if (AffixTextStyle)
		{
			AffixText_TextBlock->SetStyle(AffixTextStyle);
		}

#if WITH_EDITOR
		if (bOverride_DebugText && IsDesignTime())
		{
			AffixText_TextBlock->SetText(DebugText);
		}
#endif
	}
}

void UObsidianAffixRow::ShowAffixRow(const bool bShow)
{
	if (bShow)
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UObsidianAffixRow::InitializeAffixRow(const FText& InText)
{
	if (AffixText_TextBlock)
	{
		bIsOccupied = true;
		ShowAffixRow(true);
		AffixText_TextBlock->SetText(InText);
	}
}

bool UObsidianAffixRow::IsFree() const
{
	return !bIsOccupied;
}



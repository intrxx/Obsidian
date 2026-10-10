// Copyright 2026 out of sCope team - intrxx

#include "UI/CharacterStatus/Subwidgets/ObsidianAttributeToolTip.h"

#include "CommonTextBlock.h"


void UObsidianAttributeToolTip::InitAttributeToolTip(const FText& InToolTipText) const
{
	if(AttributeDesc_TextBlock)
	{
		AttributeDesc_TextBlock->SetText(InToolTipText);
	}
}

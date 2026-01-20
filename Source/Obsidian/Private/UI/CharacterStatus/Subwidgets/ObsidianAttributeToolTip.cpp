// Copyright 2026 out of sCope team - intrxx

#include "UI/CharacterStatus/Subwidgets/ObsidianAttributeToolTip.h"

// ~ Core
#include "CommonTextBlock.h"

// ~ Project

void UObsidianAttributeToolTip::InitAttributeToolTip(const FText& InToolTipText) const
{
	if(AttributeDesc_TextBlock)
	{
		AttributeDesc_TextBlock->SetText(InToolTipText);
	}
}

// Copyright 2026 out of sCope team - intrxx

#include "UI/GameTabsMenu/Subwidgets/ObsidianGameTabTooltip.h"

// ~ Core
#include "CommonTextBlock.h"
#include "Components/Spacer.h"

// ~ Project

void UObsidianGameTabTooltip::NativePreConstruct()
{
	Super::NativePreConstruct();

	Name_TextBlock->SetText(TabName);
	ButtonPrompt_TextBlock->SetText(TabButton);
	TextAndButton_Spacer->SetSize(TextAndButtonSpacerSize);
}


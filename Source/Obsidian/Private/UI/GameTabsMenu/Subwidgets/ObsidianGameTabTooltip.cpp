// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/GameTabsMenu/Subwidgets/ObsidianGameTabTooltip.h"
#include "CommonTextBlock.h"
#include "Components/Spacer.h"

void UObsidianGameTabTooltip::NativePreConstruct()
{
	Super::NativePreConstruct();

	Name_TextBlock->SetText(TabName);
	ButtonPrompt_TextBlock->SetText(TabButton);
	TextAndButton_Spacer->SetSize(TextAndButtonSpacerSize);
}


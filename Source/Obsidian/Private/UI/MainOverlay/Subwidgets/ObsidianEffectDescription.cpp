// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/MainOverlay/Subwidgets/ObsidianEffectDescription.h"

// ~ Core
#include "CommonTextBlock.h"

// ~ Project

void UObsidianEffectDescription::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(EffectDesc_TextBlock)
	{
		EffectDesc_TextBlock->SetWrapTextWidth(WrapTextAtNumber);
	}
}

void UObsidianEffectDescription::InitEffectDesc(const FText& InEffectName, const FText& InEffectDesc)
{
	if(EffectName_TextBlock && EffectDesc_TextBlock)
	{
		EffectName_TextBlock->SetText(InEffectName);
		EffectDesc_TextBlock->SetText(InEffectDesc);
	}
}


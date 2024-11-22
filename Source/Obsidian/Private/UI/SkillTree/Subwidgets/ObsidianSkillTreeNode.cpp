// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/PassiveSkillTree/Subwidgets/ObsidianSkillTreeNode.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

void UObsidianSkillTreeNode::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	Root_SizeBox->SetWidthOverride(SizeBoxWidth);
	Root_SizeBox->SetHeightOverride(SizeBoxHeight);

	Border_Image->SetBrush(BorderBrush);
	
	Skill_Button->SetStyle(SkillButtonStyle);
}

// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/SkillTree/ObsidianSkillTreeNode.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"

void UObsidianSkillTreeNode::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	Root_SizeBox->SetWidthOverride(SizeBoxWidth);
	Root_SizeBox->SetHeightOverride(SizeBoxHeight);

	Skill_Button->SetStyle(SkillButtonStyle);
}

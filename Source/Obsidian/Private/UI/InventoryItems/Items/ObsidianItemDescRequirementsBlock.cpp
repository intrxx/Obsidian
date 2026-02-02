// Copyright 2026 out of sCope team - intrxx

#include "UI/InventoryItems/Items/ObsidianItemDescRequirementsBlock.h"

#include <CommonTextBlock.h>

#include "Components/HorizontalBox.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

void UObsidianItemDescRequirementsBlock::InitializeRequirementsBlock(const FObsidianItemRequirementsUIDescription& RequirementsUIDescription)
{
	if (!SufficientRequirementText_Style || !InsufficientRequirementText_Style || !SufficientRequirementMagnitude_Style || !InsufficientRequirementMagnitude_Style)
	{
		return;
	}
	
	if (RequirementsUIDescription.bHasHeroClassRequirement && ClassRequirement_HorizontalBox && ClassRequirement_TextBlock)
	{
		const TSubclassOf<UCommonTextStyle> ClassReqStyle = RequirementsUIDescription.bMeetHeroClassRequirement ?
				SufficientRequirementText_Style : InsufficientRequirementText_Style;
			
		ClassRequirement_TextBlock->SetStyle(ClassReqStyle);
		ClassRequirement_TextBlock->SetText(RequirementsUIDescription.HeroClassRequirementText);
		ClassRequirement_HorizontalBox->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (RequirementsUIDescription.bHasLevelRequirement && LevelRequirement_HorizontalBox && LevelRequirement_TextBlock)
	{
		const TSubclassOf<UCommonTextStyle> LevelReqStyle = RequirementsUIDescription.bMeetLevelRequirement ?
				SufficientRequirementMagnitude_Style : InsufficientRequirementMagnitude_Style;
		
		LevelRequirement_TextBlock->SetStyle(LevelReqStyle);
		LevelRequirement_TextBlock->SetText(FText::AsNumber(RequirementsUIDescription.LevelRequirement));
		LevelRequirement_HorizontalBox->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	
	uint8 AttributeRequiredCounter = 0;
	SetupAttributeRequirement(RequirementsUIDescription.bHasStrengthRequirement, RequirementsUIDescription.bMeetStrengthRequirement,
		RequirementsUIDescription.StrengthRequirement, Strength_HorizontalBox, StrengthRequirement_TextBlock, AttributeRequiredCounter);
	SetupAttributeRequirement(RequirementsUIDescription.bHasDexterityRequirement, RequirementsUIDescription.bMeetDexterityRequirement,
		RequirementsUIDescription.DexterityRequirement, Dexterity_HorizontalBox, DexterityRequirement_TextBlock, AttributeRequiredCounter);
	SetupAttributeRequirement(RequirementsUIDescription.bHasFaithRequirement, RequirementsUIDescription.bMeetFaithRequirement,
		RequirementsUIDescription.FaithRequirement, Faith_HorizontalBox, FaithRequirement_TextBlock, AttributeRequiredCounter);
	SetupAttributeRequirement(RequirementsUIDescription.bHasIntelligenceRequirement, RequirementsUIDescription.bMeetIntelligenceRequirement,
		RequirementsUIDescription.IntelligenceRequirement, Intelligence_HorizontalBox, IntelligenceRequirement_TextBlock, AttributeRequiredCounter);
	
	if (AttributeRequiredCounter > 0 && AttributeRequirements_HorizontalBox)
	{
		AttributeRequirements_HorizontalBox->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UObsidianItemDescRequirementsBlock::SetupAttributeRequirement(const bool bHasRequirement, const bool bMeetRequirement,
	const int32 RequirementValue, UHorizontalBox* RequirementContainer, UCommonTextBlock* RequirementText, uint8& Counter)
{
	if (bHasRequirement && RequirementContainer && RequirementText)
	{
		const TSubclassOf<UCommonTextStyle> AttributeReqStyle = bMeetRequirement ? SufficientRequirementMagnitude_Style :
			InsufficientRequirementMagnitude_Style;
		
		RequirementText->SetStyle(AttributeReqStyle);
		RequirementText->SetText(FText::AsNumber(RequirementValue));
		RequirementContainer->SetVisibility(ESlateVisibility::HitTestInvisible);
		++Counter;
	}
}

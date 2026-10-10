// Copyright 2026 out of sCope team - intrxx

#include "UI/InventoryItems/Items/ObsidianItemDescRequirementsBlock.h"

#include "CommonTextBlock.h"
#include "Components/HorizontalBox.h"

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"


void UObsidianItemDescRequirementsBlock::InitializeRequirementsBlock(const FObsidianItemRequirementsUIDescription& InRequirementsUIDescription)
{
	if (!SufficientRequirementText_Style || !InsufficientRequirementText_Style || !SufficientRequirementMagnitude_Style || !InsufficientRequirementMagnitude_Style)
	{
		return;
	}
	
	if (InRequirementsUIDescription.bHasHeroClassRequirement && ClassRequirement_HorizontalBox && ClassRequirement_TextBlock)
	{
		const TSubclassOf<UCommonTextStyle> ClassReqStyle = InRequirementsUIDescription.bMeetHeroClassRequirement ?
				SufficientRequirementText_Style : InsufficientRequirementText_Style;
			
		ClassRequirement_TextBlock->SetStyle(ClassReqStyle);
		ClassRequirement_TextBlock->SetText(InRequirementsUIDescription.HeroClassRequirementText);
		ClassRequirement_HorizontalBox->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (InRequirementsUIDescription.bHasLevelRequirement && LevelRequirement_HorizontalBox && LevelRequirement_TextBlock)
	{
		const TSubclassOf<UCommonTextStyle> LevelReqStyle = InRequirementsUIDescription.bMeetLevelRequirement ?
				SufficientRequirementMagnitude_Style : InsufficientRequirementMagnitude_Style;
		
		LevelRequirement_TextBlock->SetStyle(LevelReqStyle);
		LevelRequirement_TextBlock->SetText(FText::AsNumber(InRequirementsUIDescription.LevelRequirement));
		LevelRequirement_HorizontalBox->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	
	uint8 AttributeRequiredCounter = 0;
	SetupAttributeRequirement(InRequirementsUIDescription.bHasStrengthRequirement, InRequirementsUIDescription.bMeetStrengthRequirement,
		InRequirementsUIDescription.StrengthRequirement, Strength_HorizontalBox, StrengthRequirement_TextBlock, AttributeRequiredCounter);
	SetupAttributeRequirement(InRequirementsUIDescription.bHasDexterityRequirement, InRequirementsUIDescription.bMeetDexterityRequirement,
		InRequirementsUIDescription.DexterityRequirement, Dexterity_HorizontalBox, DexterityRequirement_TextBlock, AttributeRequiredCounter);
	SetupAttributeRequirement(InRequirementsUIDescription.bHasFaithRequirement, InRequirementsUIDescription.bMeetFaithRequirement,
		InRequirementsUIDescription.FaithRequirement, Faith_HorizontalBox, FaithRequirement_TextBlock, AttributeRequiredCounter);
	SetupAttributeRequirement(InRequirementsUIDescription.bHasIntelligenceRequirement, InRequirementsUIDescription.bMeetIntelligenceRequirement,
		InRequirementsUIDescription.IntelligenceRequirement, Intelligence_HorizontalBox, IntelligenceRequirement_TextBlock, AttributeRequiredCounter);
	
	if (AttributeRequiredCounter > 0 && AttributeRequirements_HorizontalBox)
	{
		AttributeRequirements_HorizontalBox->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UObsidianItemDescRequirementsBlock::SetupAttributeRequirement(const bool bInHasRequirement, const bool bInMeetRequirement,
	const int32 InRequirementValue, UHorizontalBox* InRequirementContainer, UCommonTextBlock* InRequirementText, uint8& InOutCounter)
{
	if (bInHasRequirement && InRequirementContainer && InRequirementText)
	{
		const TSubclassOf<UCommonTextStyle> AttributeReqStyle = bInMeetRequirement ? SufficientRequirementMagnitude_Style :
			InsufficientRequirementMagnitude_Style;
		
		InRequirementText->SetStyle(AttributeReqStyle);
		InRequirementText->SetText(FText::AsNumber(InRequirementValue));
		InRequirementContainer->SetVisibility(ESlateVisibility::HitTestInvisible);
		++InOutCounter;
	}
}

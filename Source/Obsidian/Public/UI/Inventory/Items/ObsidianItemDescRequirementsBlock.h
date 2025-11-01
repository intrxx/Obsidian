// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include "UI/ObsidianWidgetBase.h"
#include "ObsidianItemDescRequirementsBlock.generated.h"

struct FObsidianItemRequirementsUIDescription;

class UCommonTextStyle;
class UCommonTextBlock;
class UHorizontalBox;

/**
  * Widget component that displays item requirements (class, level, attributes) with visual styling to indicate whether
  * requirements are met or insufficient.
 */
UCLASS()
class OBSIDIAN_API UObsidianItemDescRequirementsBlock : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void InitializeRequirementsBlock(const FObsidianItemRequirementsUIDescription& RequirementsUIDescription);

protected:
	void SetupAttributeRequirement(const bool bHasRequirement, const bool bMeetRequirement, const int32 RequirementValue,
		UHorizontalBox* RequirementContainer, UCommonTextBlock* RequirementText, uint8& Counter);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Style")
	TSubclassOf<UCommonTextStyle> SufficientRequirementText_Style;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Style")
	TSubclassOf<UCommonTextStyle> SufficientRequirementMagnitude_Style;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Style")
	TSubclassOf<UCommonTextStyle> InsufficientRequirementText_Style;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Style")
	TSubclassOf<UCommonTextStyle> InsufficientRequirementMagnitude_Style;
	
protected:
	/**
	 * Class Requirement
	 */
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> ClassRequirement_HorizontalBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ClassRequirement_TextBlock;

	/**
	 * Level Requirement
	 */
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> LevelRequirement_HorizontalBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> LevelRequirement_TextBlock;

	/**
	 * Attributes Requirements
	 */

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> AttributeRequirements_HorizontalBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> Strength_HorizontalBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> StrengthRequirement_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> Dexterity_HorizontalBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> DexterityRequirement_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> Faith_HorizontalBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> FaithRequirement_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> Intelligence_HorizontalBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> IntelligenceRequirement_TextBlock;
};

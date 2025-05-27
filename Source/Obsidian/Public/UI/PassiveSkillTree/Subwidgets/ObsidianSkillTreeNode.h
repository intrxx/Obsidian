// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UI/ObsidianWidgetBase.h"
#include "ObsidianSkillTreeNode.generated.h"

class UButton;
class UImage;
class UOverlay;
class USizeBox;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianSkillTreeNode : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	
public:
	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SizeBoxWidth = 0;
	
	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SizeBoxHeight = 0;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	FButtonStyle SkillButtonStyle = FButtonStyle();

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	FSlateBrush BorderBrush = FSlateBrush();
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Main_Overlay;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Border_Image;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Skill_Button;
};

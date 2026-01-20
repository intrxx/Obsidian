// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UI/ObsidianWidgetBase.h"
#include "ObsidianGameTabButton.generated.h"

class UObsidianGameTabTooltip;
class UButton;
class UImage;
class UOverlay;
class USizeBox;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianGameTabButton : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnTabButtonHovered();

	UFUNCTION()
	void OnTabButtonUnhovered();
	
public:
	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SizeBoxWidth = 0;
	
	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SizeBoxHeight = 0;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	FButtonStyle TabButtonStyle = FButtonStyle();

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	FSlateBrush BorderBrush = FSlateBrush();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Tab_Button;

	bool bIsCorrespondingTabOpen = false;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Main_Overlay;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Border_Image;
	
	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianGameTabTooltip> TabToolTipWidgetClass;

	UPROPERTY()
	TObjectPtr<UObsidianGameTabTooltip> TabToolTipWidget;
};

// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "UI/Subwidgets/ObsidianToolTipBase.h"

#include "ObsidianGameTabTooltip.generated.h"

class USpacer;
class UCommonTextBlock;
class UCommonTextStyle;
class UHorizontalBox;
class USizeBox;

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API UObsidianGameTabTooltip : public UObsidianToolTipBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Obsidian|ToolTip")
	FText TabButton = FText();

	UPROPERTY(EditAnywhere, Category = "Obsidian|ToolTip")
	FText TabName = FText();

	UPROPERTY(EditAnywhere, Category = "Obsidian|ToolTip")
	FVector2D TextAndButtonSpacerSize = FVector2D(1.0f, 1.0f);

protected:
	//~ Begin UUserWidget Interface
	virtual void NativePreConstruct() override;
	//~ End UUserWidget Interface

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> Main_HorizontalBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> Name_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USpacer> TextAndButton_Spacer;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ButtonPrompt_TextBlock;
};

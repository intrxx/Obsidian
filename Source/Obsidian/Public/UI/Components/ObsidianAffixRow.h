// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include "UI/ObsidianWidgetBase.h"
#include "ObsidianAffixRow.generated.h"

class UCommonTextStyle;
class UCommonTextBlock;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianAffixRow : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void InitializeAffixRow(const FText& InText);

	bool IsFree() const;

protected:
	virtual void NativePreConstruct() override;

	void RefreshAffixRowStyle();
	void ShowAffixRow(const bool bShow);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> AffixText_TextBlock;

private:
	UPROPERTY(EditAnywhere, Category = "Obsidian|AffixRow")
	TSubclassOf<UCommonTextStyle> AffixTextStyle;
	
	bool bIsOccupied = false;

	UPROPERTY(EditAnywhere, Meta = (InlineEditConditionToggle),Category = "Obsidian|AffixRow")
	uint8 bOverride_DebugText:1;

	/** This is just optional Debug Text representation of the Affix, won't affect anything that is displayed in runtime. */
	UPROPERTY(EditAnywhere, Meta = (EditCondition = "bOverride_DebugText"), Category = "Obsidian|AffixRow")
	FText DebugText;
};

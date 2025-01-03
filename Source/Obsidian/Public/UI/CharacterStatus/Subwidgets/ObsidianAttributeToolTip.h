// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/Subwidgets/ObsidianToolTipBase.h"
#include "ObsidianAttributeToolTip.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianAttributeToolTip : public UObsidianToolTipBase
{
	GENERATED_BODY()

public:
	void InitAttributeToolTip(const FText& InToolTipText) const;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> AttributeDesc_TextBlock;
};

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianToolTipBase.generated.h"

class UCommonTextBlock;
class USizeBox;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianToolTipBase : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	virtual FVector2D GetDesiredPosition() const;
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|ToolTip")
	FVector2D ToolTipOffset = {0, 0};
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;
};

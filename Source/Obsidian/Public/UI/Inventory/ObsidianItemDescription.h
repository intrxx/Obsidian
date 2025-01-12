// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianItemDescription.generated.h"

class UCommonTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemDescription : public UObsidianWidgetBase
{
	GENERATED_BODY()

protected:
UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Background_Image;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ItemName_TextBlock;


	
};

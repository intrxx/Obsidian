// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianItemDescriptionBase.generated.h"

class UCommonTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemDescriptionBase : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void SetItemDisplayName(const FText& DisplayName);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ItemName_Image;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ItemName_TextBlock;
};

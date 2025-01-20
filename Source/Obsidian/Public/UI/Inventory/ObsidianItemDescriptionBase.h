// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianItemDescriptionBase.generated.h"

class UHorizontalBox;
struct FObsidianItemStats;
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
	void InitializeWidgetWithItemStats(const FObsidianItemStats& ItemStats);
	
	void SetItemDisplayName(const FText& DisplayName);
	void SetStackCount(const int32 CurrentStacks, const int32 MaxStacks);
	void SetItemDescription(const FText& ItemDescription);
	void SetAdditionalItemDescription(const FText& AdditionalItemDescription);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ItemName_Image;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ItemName_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> StacksContainer_HorizontalBox;
	
	UPROPERTY(meta=(BindWidget))	
	TObjectPtr<UCommonTextBlock> StackCount_TextBlock;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ItemDescription_TextBlock;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> AdditionalItemDescription_TextBlock;

private:
	void CollapseStatBlocks();
};


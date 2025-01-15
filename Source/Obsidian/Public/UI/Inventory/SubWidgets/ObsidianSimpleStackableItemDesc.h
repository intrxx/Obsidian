// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/Inventory/ObsidianItemDescriptionBase.h"
#include "ObsidianSimpleStackableItemDesc.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianSimpleStackableItemDesc : public UObsidianItemDescriptionBase
{
	GENERATED_BODY()

public:
	void SetStackCount(const int32 CurrentStacks, const int32 MaxStacks);
	void SetItemDescription(const FText& ItemDescription);
	void SetAdditionalItemDescription(const FText& AdditionalItemDescription);

protected:
	UPROPERTY(meta=(BindWidget))	
	TObjectPtr<UCommonTextBlock> StackCount_TextBlock;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ItemDescription_TextBlock;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> AdditionalItemDescription_TextBlock;
};

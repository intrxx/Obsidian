// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianDraggedItem.generated.h"

class UObsidianInventoryItemInstance;
class UObsidianInventoryItemDefinition;
class USizeBox;
class UCommonTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianDraggedItem : public UObsidianWidgetBase
{
	GENERATED_BODY()
	
public:
	void InitializeItemWidgetWithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 Stacks);
	void InitializeItemWidgetWithItemInstance(const UObsidianInventoryItemInstance* ItemInstance);
	
	/** Updates the stack count on the dragged item. */
	void UpdateStackCount(const int32 NewStackCount);
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Item_Image;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> StackCount_TextBlock;

protected:
	virtual void NativeConstruct() override;

protected:
	int32 InternalStacks = 0;

private:
	bool bStackableItem = false;
};

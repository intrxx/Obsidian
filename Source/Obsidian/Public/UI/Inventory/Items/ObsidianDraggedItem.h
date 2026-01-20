// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

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
	void InitializeItemWidgetWithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FObsidianItemGeneratedData& GeneratedData);
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

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
	void InitializeItemWidgetWithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 Stacks);
	void InitializeItemWidgetWithItemInstance(UObsidianInventoryItemInstance* ItemInstance);

	void UpdateStackCount(const int32 NewStackCount);

	/** Invalid when widget initialized with Instance. */
	TSubclassOf<UObsidianInventoryItemDefinition> GetItemDef() const
	{
		return InternalItemDef;
	}
	/** Invalid when widget initialized with Definition. */
	UObsidianInventoryItemInstance* GetItemInstance() const
	{
		return InternalItemInstance;
	}
	/** Invalid when widget initialized with Definition. */
	int32 GetItemStacks() const
	{
		return InternalStacks;
	}
	
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
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	float WidthConstant = 64.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	float HeightConstant = 64.0f;
	
	TSubclassOf<UObsidianInventoryItemDefinition> InternalItemDef;
	int32 InternalStacks = 0;

	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemInstance> InternalItemInstance;
};

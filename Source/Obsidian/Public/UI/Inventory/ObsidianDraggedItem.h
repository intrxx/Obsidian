// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianDraggedItem.generated.h"

class UObsidianInventoryItemDefinition;
class USizeBox;
class UImage;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianDraggedItem : public UObsidianWidgetBase
{
	GENERATED_BODY()
	
public:
	void InitializeItemWidget(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 Stacks);
	
	TSubclassOf<UObsidianInventoryItemDefinition> GetItemDef() const
	{
		return InternalItemDef;
	}
	int32 GetItemStacks() const
	{
		return InternalStacks;
	}
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Item_Image;

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	float WidthConstant = 64.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	float HeightConstant = 64.0f;
	
	TSubclassOf<UObsidianInventoryItemDefinition> InternalItemDef;
	int32 InternalStacks = 0;
};

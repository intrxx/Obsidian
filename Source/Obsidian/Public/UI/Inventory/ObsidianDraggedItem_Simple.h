// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UI/ObsidianWidgetBase.h"
#include "ObsidianDraggedItem_Simple.generated.h"

class USizeBox;
class UImage;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianDraggedItem_Simple : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void InitializeDraggedItem(const FSlateBrush& InItemImage, const FIntPoint& InItemGridSpan);
	
	FVector2D GetItemWidgetSize() const;
	
protected:
	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float ItemDownsizeScale = 0.45f;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Item_Image;
};

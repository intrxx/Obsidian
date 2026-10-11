// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "UI/ObsidianWidgetBase.h"

#include "ObsidianDraggedItem_Simple.generated.h"

class USizeBox;
class UImage;

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API UObsidianDraggedItem_Simple : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void InitializeDraggedItem(const FSlateBrush& InItemImage, const FIntPoint& InItemGridSpan);

	FVector2D GetItemWidgetSize() const;

protected:
	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	//~ End UUserWidget Interface

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float ItemDownsizeScale = 0.45f;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Item_Image;
};

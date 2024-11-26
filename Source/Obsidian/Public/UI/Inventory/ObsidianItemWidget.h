// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianItemWidget.generated.h"

class USizeBox;
class UImage;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemWidget : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void InitializeItemWidget(const FVector2D& ItemGridSpan, UTexture2D* ItemImage) const;
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Item_Image;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	float WidthConstant = 64.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	float HeightConstant = 64.0f;
};

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianItemWidget.generated.h"

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
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Item_Image;
};

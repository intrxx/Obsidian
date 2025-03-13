// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianItem.generated.h"

class UCommonTextBlock;
class USizeBox;
class UObsidianItem;
class UImage;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnItemLeftMouseButtonPressedSignature, const FVector2D& ItemDesiredPosition, UObsidianItem* ItemWidget, const bool bShiftPressed);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemRightMouseButtonPressedSignature, const FVector2D& ItemDesiredPosition, UObsidianItem* ItemWidget);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemMouseEnterSignature, const FVector2D& ItemPosition, const UObsidianItem* ItemWidget);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemMouseLeaveSignature, const FVector2D& ItemPosition);

/**
 * Item Widget that is displayed in the inventory.
 */
UCLASS()
class OBSIDIAN_API UObsidianItem : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void InitializeItemWidget(const FVector2D& DesiredPosition, const FVector2D& ItemGridSpan, UTexture2D* ItemImage, const int32 CurrentStack = 0);
	void AddCurrentStackCount(const int32 StackCountToAdd);
	void OverrideCurrentStackCount(const int32 NewStackCount);

	FVector2D GetItemSize() const;
	void SetSize(const FVector2D& ItemGridSpan);
	
public:
	FOnItemLeftMouseButtonPressedSignature OnItemLeftMouseButtonPressedDelegate;
	FOnItemRightMouseButtonPressedSignature OnItemRightMouseButtonPressedDelegate;
	FOnItemMouseEnterSignature OnItemMouseEnterDelegate;
	FOnItemMouseLeaveSignature OnItemMouseLeaveDelegate;
	
protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Item_Image;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> StackCount_TextBlock;
	
	FVector2D ItemDesiredPosition = FVector2D::Zero();

private:
	int32 InternalStacks = 0;
};

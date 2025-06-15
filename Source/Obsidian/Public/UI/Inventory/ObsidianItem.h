// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project


#include "UI/ObsidianWidgetBase.h"
#include "ObsidianItem.generated.h"

class UCommonTextBlock;
class USizeBox;
class UObsidianItem;
class UImage;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemLeftMouseButtonPressedSignature, const UObsidianItem* ItemWidget, const bool bShiftPressed);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRightMouseButtonPressedSignature, UObsidianItem* ItemWidget);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemMouseEnterSignature, const UObsidianItem* ItemWidget);
DECLARE_MULTICAST_DELEGATE(FOnItemMouseLeaveSignature);

/**
 * Item Widget that is displayed in the inventory.
 */
UCLASS()
class OBSIDIAN_API UObsidianItem : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void InitializeItemWidget(const FIntPoint& DesiredPosition, const FIntPoint& ItemGridSpan, UTexture2D* ItemImage, const int32 CurrentStack = 0);
	void InitializeItemWidget(const FGameplayTag& EquipmentSlot, const FIntPoint& ItemGridSpan, UTexture2D* ItemImage);
	
	void AddCurrentStackCount(const int32 StackCountToAdd);
	void OverrideCurrentStackCount(const int32 NewStackCount);

	FIntPoint GetInventoryPosition() const
	{
		return ItemDesiredPosition;
	}
	
	FIntPoint GetItemGridSpan() const
	{
		return ItemDesiredGridSpan;
	}
	
	FGameplayTag GetEquipmentSlotTag() const
	{
		return ItemEquipmentSlot;
	}

	FSlateBrush GetItemImage() const;

	FVector2D GetItemWidgetSize() const;
	void SetSize(const FIntPoint& ItemGridSpan);

	void SetUsingItemProperties();
	void ResetUsingItemProperties();

	void HighlightItem();
	void ResetHighlight();
	
public:
	FOnItemLeftMouseButtonPressedSignature OnItemLeftMouseButtonPressedDelegate;
	FOnItemRightMouseButtonPressedSignature OnItemRightMouseButtonPressedDelegate;
	FOnItemMouseEnterSignature OnItemMouseEnterDelegate;
	FOnItemMouseLeaveSignature OnItemMouseLeaveDelegate;
	
protected:
	virtual void NativeConstruct() override;
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
protected:
	/** Opacity to set when item is being used (right-clicked). */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float UsingItemOpacity = 0.6f;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Highlight_Image;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Item_Image;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> StackCount_TextBlock;

	UPROPERTY()
	FIntPoint ItemDesiredPosition = FIntPoint::NoneValue;
	
	UPROPERTY()
	FIntPoint ItemDesiredGridSpan = FIntPoint::NoneValue;

	UPROPERTY()
	FGameplayTag ItemEquipmentSlot = FGameplayTag::EmptyTag;

private:
	int32 InternalStacks = 0;
};

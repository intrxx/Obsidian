// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "UI/ObsidianWidgetBase.h"
#include "ObsidianUnstackSlider.generated.h"

class USizeBox;
class USlider;
class UCommonTextBlock;
class UButton;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAcceptButtonPressedSignature, const int32 StacksToTake, const FObsidianItemPosition& ItemPosition)
DECLARE_MULTICAST_DELEGATE(FOnCloseButtonPressedSignature)

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianUnstackSlider : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void InitializeUnstackSlider(const int32 CurrentItemStacks, const FObsidianItemPosition& InItemPosition);

	FVector2D GetSizeBoxSize() const;

	void DestroyUnstackSlider();

public:
	FOnAcceptButtonPressedSignature OnAcceptButtonPressedDelegate;
	FOnCloseButtonPressedSignature OnCloseButtonPressedDelegate;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void OnCloseButtonClicked();
	
	UFUNCTION()
	void OnAcceptButtonClicked();

	UFUNCTION()
	void UpdateStacksValues(float NewValue);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Close_Button;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Accept_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> StacksToLeave_TextBlock;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> StacksToTake_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USlider> Stacks_Slider;

private:
	int32 StackToLeave;
	int32 StackToTake;
	int32 MaxStacks;

	UPROPERTY()
	FObsidianItemPosition ItemSlotPosition = FObsidianItemPosition();
};

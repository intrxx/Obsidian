// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianUnstackSlider.generated.h"

class USizeBox;
class USlider;
class UCommonTextBlock;
class UButton;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAcceptButtonPressedSignature, const int32 StacksToTake)

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianUnstackSlider : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void InitializeUnstackSlider(const int32 CurrentItemStacks);

	FVector2D GetSizeBoxSize() const;
	float GetTopDesiredOffset() const;

	void DestroyUnstackSlider();

public:
	FOnAcceptButtonPressedSignature OnAcceptButtonPressedDelegate;

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
	/** Offset from the top of the spawned above stackable item widget. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float TopDesiredOffset = 5.0f;
	
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
};
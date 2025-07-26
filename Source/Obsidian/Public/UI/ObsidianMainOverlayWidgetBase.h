// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "ObsidianWidgetBase.h"
#include "ObsidianMainOverlayWidgetBase.generated.h"

class UButton;
class USizeBox;

/** Fired when a widget is destroyed, used to fire logic when widget was destroyed via other method than it was opened with. */
DECLARE_MULTICAST_DELEGATE(FOnWidgetDestroyedSignature);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMainOverlayWidgetBase : public UObsidianWidgetBase
{
	GENERATED_BODY()
	
public:
	float GetWindowWidth() const
	{
		return RootSizeBoxWidth;
	}

	float GetWindowHeight() const
	{
		return RootSizeBoxHeight;
	}
	
public:
	FOnWidgetDestroyedSignature OnWidgetDestroyedDelegate;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	UFUNCTION()
	void OnCloseButtonClicked();
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Close_Button;

	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle), Category = "Obsidian|Setup")
	uint8 bOverride_RootSizeHeight : 1;
	
	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle), Category = "Obsidian|Setup")
	uint8 bOverride_RootSizeWidth : 1;

	/** Essentially, height component of inventory size. Use this instead of directly setting it on SizeBox. */
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bOverride_RootSizeHeight"), Category = "Obsidian|Setup")
	float RootSizeBoxHeight = 0.0f;

	/** Essentially, width component of inventory size. Use this instead of directly setting it on SizeBox. */
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bOverride_RootSizeWidth"), Category = "Obsidian|Setup")
	float RootSizeBoxWidth = 0.0f;
};

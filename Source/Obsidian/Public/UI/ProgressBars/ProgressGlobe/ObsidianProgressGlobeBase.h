// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ProgressBars/ObsidianProgressBarBase.h"
#include "Styling/SlateBrush.h"
#include "Layout/Margin.h"
#include "ObsidianProgressGlobeBase.generated.h"

class UMainOverlayWidgetController;
class UHorizontalBox;
class UProgressBar;
class UCommonTextBlock;
class UImage;
class UOverlay;
class USizeBox;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianProgressGlobe : public UObsidianProgressBarBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|ProgressGlobe")
	void SetInfoGlobeVisibility(const bool bShouldBeVisible);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|ProgressGlobe")
	void SetSecondAttributeName(const FText& SecondAttributeNameToSet);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|ProgressGlobe")
	void SetProgressGlobeStyle(const FSlateBrush& ProgressGlobeFillImage);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|ProgressGlobe")
	void ResetStyle();

public:
	/**
	 *  Widget build blocks
	 */
	
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<USizeBox> RootSizeBox;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UOverlay> RootOverlay;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UImage> GlobeWrapperImage;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UImage> GlobeGlassImage;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressGlobe;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UProgressBar> GhostProgressGlobe;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UProgressBar> InfoProgressGlobe;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> FirstAttributeNameText;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> FirstAttributeCountText;
	
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> SecondAttributeNameText;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> SecondAttributeCountText;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UHorizontalBox> EnergyShieldAttributeValueBox;
	
	/**
	 * Set up
	 */
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|Setup")
	FSlateBrush GlobeWrapperBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|Setup|Globe")
	FSlateBrush GlobeFillImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|Setup|Globe")
	FSlateBrush GhostGlobeFillImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|Setup|Globe")
	FSlateBrush InfoGlobeFillImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|Setup|Globe")
	FMargin GlobeMargin;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|Setup")
	float RootBoxWidth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|Setup")
	float RootBoxHeight = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|Setup|Attributes")
	FText FirstAttributeName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|Setup|Attributes")
	FText SecondAttributeName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Obsidian")
	bool bShouldSetGhostGlobe = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Obsidian")
	bool bShouldShowInfoGlobe = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian")
	float GhostGlobeFollowingSpeed = 5.f;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void HandleWidgetControllerSet() override;
	virtual void NativePreConstruct() override;

	void ShouldGhostGlobeDecrease(const float NewAttribute, const float Attribute, const float MaxAttribute);
	
protected:
	UPROPERTY()
	TObjectPtr<UMainOverlayWidgetController> MainOverlayWidgetController;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CurrentPercentage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float NewPercentage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian")
	bool bInfoGlobeActive = false;

private:
	void SetGhostGlobeDecreasing(const float CurrentPercent, const float NewPercent, const float DeltaTime);
	
};


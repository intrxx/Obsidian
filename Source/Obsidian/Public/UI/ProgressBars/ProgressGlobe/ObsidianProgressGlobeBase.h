// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "Layout/Margin.h"
#include "Styling/SlateBrush.h"

#include "UI/ProgressBars/ObsidianProgressBarBase.h"

#include "ObsidianProgressGlobeBase.generated.h"

class UObMainOverlayWidgetController;
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
	void SetInfoGlobeVisibility(const bool bInShouldBeVisible);

public:
	/**
	 *  Widget build blocks
	 */
	
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<USizeBox> RootSizeBox;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UOverlay> RootOverlay;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UImage> GlobeWrapper_Image;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UImage> GlobeGlass_Image;
	
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UProgressBar> Ghost_ProgressGlobe;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Setup", meta=(BindWidget))
	TObjectPtr<UProgressBar> Info_ProgressGlobe;
	
	/**
	 * Set up
	 */
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|Setup|Globe")
	FSlateBrush GlobeFillImage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Obsidian")
	bool bShouldSetGhostGlobe = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Obsidian")
	bool bShouldShowInfoGlobe = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian")
	float GhostGlobeFollowingSpeed = 5.f;

protected:
	virtual void NativeTick(const FGeometry& InMyGeometry, float InDeltaTime) override;
	virtual void HandleWidgetControllerSet() override;
	virtual void NativePreConstruct() override;

	void ShouldGhostGlobeDecrease(const float InNewAttribute, const float InAttribute, const float InMaxAttribute);
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
protected:
	UPROPERTY()
	TObjectPtr<UObMainOverlayWidgetController> MainOverlayWidgetController;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CurrentPercentage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float NewPercentage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian")
	bool bInfoGlobeActive = false;

private:
	void SetGhostGlobeDecreasing(const float InCurrentPercent, const float InNewPercent, const float InDeltaTime);
	
};


// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"
#include "Layout/Margin.h"

// ~ Project


#include "UI/ProgressBars/ObsidianProgressBarBase.h"
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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void HandleWidgetControllerSet() override;
	virtual void NativePreConstruct() override;

	void ShouldGhostGlobeDecrease(const float NewAttribute, const float Attribute, const float MaxAttribute);
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
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


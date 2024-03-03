// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "Styling/SlateBrush.h"
#include "Layout/Margin.h"
#include "ObsidianProgressGlobe.generated.h"

class UProgressBar;
class UImage;
class UOverlay;
class USizeBox;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianProgressGlobe : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|ProgressGlobe")
	void SetProgressGlobePercent(float Percent);
	
public:
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|Setup")
	float RootBoxWidth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|Setup")
	float RootBoxHeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|Setup")
	FSlateBrush GlobeWrapperBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|Setup|Globe")
	FSlateBrush GlobeFillImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obsidian|Setup|Globe")
	FMargin GlobeMargin;
};


// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Blueprint/UserWidget.h"
#include "ObsidianRadialProgressBar.generated.h"

class USizeBox;
class UImage;

/**
 * 
 */
UCLASS(NotBlueprintType)
class OBSIDIAN_API UObsidianRadialProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "RadialProgressBar")
	void SetPercent(float InPercent);

public:
	UPROPERTY(EditAnywhere, Category = "RadialProgressBar",  meta = (UIMin = "0", UIMax = "1"))
	float Percent = 0.0f;

	UPROPERTY(EditAnywhere, Category = "RadialProgressBar",  meta = (UIMin = "0", UIMax = "0.5"))
	float Thickness = 0.1f;

	UPROPERTY(EditAnywhere, Category = "RadialProgressBar",  meta = (UIMin = "0", UIMax = "1"))
	float BarPercentMin = 0.0f;

	UPROPERTY(EditAnywhere, Category = "RadialProgressBar",  meta = (UIMin = "0", UIMax = "1"))
	float BarPercentMax = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "RadialProgressBar")
	float CircularWidth = 300.0f;
	
	UPROPERTY(EditAnywhere, Category = "RadialProgressBar")
	float CircularHeight = 300.0f;
	
	UPROPERTY(EditAnywhere, Category = "RadialProgressBar")
	TObjectPtr<UMaterialInstance> ProgressBarMaterial;

	UPROPERTY(EditAnywhere, Category = "RadialProgressBar")
	FName ProgressValueName = FName("Percent");

	UPROPERTY(EditAnywhere, Category = "RadialProgressBar")
	FName ThicknessValueName = FName("Thickness");

protected:
	virtual void NativePreConstruct() override;

	bool CreateProgressBarMaterial();

	void SetThickness(const float InThickness);

	float GetMappedValue(float RawValue);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ProgressBar_Image;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ProgressBarMaterialDynamic;

private:
	float BarPercent = 0.0f;
};

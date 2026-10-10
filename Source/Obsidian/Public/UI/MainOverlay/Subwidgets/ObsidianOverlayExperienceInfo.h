// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "UI/ObsidianWidgetBase.h"

#include "ObsidianOverlayExperienceInfo.generated.h"

class UCommonTextBlock;
class USizeBox;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianOverlayExperienceInfo : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void InitializeExperienceInfo(const float InCurrentExperience, const float InMaxExperience, const float InLastMaxExperience, const int32 InPlayerLevel);
	
	void DestroyExperienceInfo();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian")
	FVector2D ScreenDisplayOffset = FVector2D(50.0f, -25.0f);

protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ExperiencePercentage_TextBlock;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ExperienceNumber_TextBlock;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ExperiencePerHour_TextBlock;
};

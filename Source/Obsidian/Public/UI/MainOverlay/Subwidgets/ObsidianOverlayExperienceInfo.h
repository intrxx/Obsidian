// Copyright 2024 out of sCope team - Michał Ogiński

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
	void InitializeExperienceInfo(const float Experience, const float MaxExperience, const float LastMaxExperience, const int32 PlayerLevel);
	
	void DestroyExperienceInfo();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian")
	FVector2D ScreenDisplayOffset = FVector2D(50.0f, -25.0f);

protected:
	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ExperienceNumber_TextBlock;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ExperiencePercentage_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ExperiencePerHour_TextBlock;
};

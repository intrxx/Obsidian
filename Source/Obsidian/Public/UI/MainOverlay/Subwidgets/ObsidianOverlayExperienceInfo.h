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
	void InitializeExperienceInfo(const float Experience, const float MaxExperience, const float LastMaxExperience);
	
	FVector2D GetSizeBoxSize() const;
	
	void DestroyExperienceInfo();

protected:
	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ExperienceNumber_TextBlock;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ExperiencePercentage_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> ExperiencePerHour_TextBlock;
};

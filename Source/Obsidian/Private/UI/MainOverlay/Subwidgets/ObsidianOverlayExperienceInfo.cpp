// Copyright 2026 out of sCope team - intrxx

#include "UI/MainOverlay/Subwidgets/ObsidianOverlayExperienceInfo.h"

#include "CommonTextBlock.h"
#include "Components/SizeBox.h"


void UObsidianOverlayExperienceInfo::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

FReply UObsidianOverlayExperienceInfo::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// This widgets won't take any input, don't want to pass gameplay input through
	return FReply::Handled();
}

FReply UObsidianOverlayExperienceInfo::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// This widgets won't take any input, don't want to pass gameplay input through
	return FReply::Handled();
}

void UObsidianOverlayExperienceInfo::InitializeExperienceInfo(const float InCurrentExperience, const float InMaxExperience, const float InLastMaxExperience, const int32 InPlayerLevel)
{
	int32 Percentage = 0;
	if(InMaxExperience > 0.0f)
	{
		Percentage = FMath::TruncToInt(((InCurrentExperience - InLastMaxExperience) / (InMaxExperience - InLastMaxExperience) * 100));
	}
	
	const FText ExperiencePercentageText = FText::FromString(FString::Printf(TEXT("%d, (%d%%) towards the next level"), InPlayerLevel, Percentage));
	if(ExperiencePercentage_TextBlock)
	{
		ExperiencePercentage_TextBlock->SetText(ExperiencePercentageText);
	}
	
	const FText ExperienceText = FText::FromString(FString::Printf(TEXT("%d out of %d experience needed"), FMath::TruncToInt(InCurrentExperience), FMath::TruncToInt(InMaxExperience)));
	if(ExperienceNumber_TextBlock)
	{
		ExperienceNumber_TextBlock->SetText(ExperienceText);
	}

	if(ExperiencePerHour_TextBlock)
	{
		ExperiencePerHour_TextBlock->SetVisibility(ESlateVisibility::Collapsed); //TODO(intrxx) Create logic for XP per h
	}
}

void UObsidianOverlayExperienceInfo::DestroyExperienceInfo()
{
	RemoveFromParent();
}



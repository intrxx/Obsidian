// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/MainOverlay/Subwidgets/ObsidianOverlayExperienceInfo.h"

// ~ Core
#include "CommonTextBlock.h"
#include "Components/SizeBox.h"

// ~ Project

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

void UObsidianOverlayExperienceInfo::InitializeExperienceInfo(const float CurrentExperience, const float MaxExperience, const float LastMaxExperience, const int32 PlayerLevel)
{
	int32 Percentage = 0;
	if(MaxExperience > 0.0f)
	{
		Percentage = FMath::TruncToInt(((CurrentExperience - LastMaxExperience) / (MaxExperience - LastMaxExperience) * 100));
	}
	
	const FText ExperiencePercentageText = FText::FromString(FString::Printf(TEXT("%d, (%d%%) towards the next level"), PlayerLevel, Percentage));
	if(ExperiencePercentage_TextBlock)
	{
		ExperiencePercentage_TextBlock->SetText(ExperiencePercentageText);
	}
	
	const FText ExperienceText = FText::FromString(FString::Printf(TEXT("%d out of %d experience needed"), FMath::TruncToInt(CurrentExperience), FMath::TruncToInt(MaxExperience)));
	if(ExperienceNumber_TextBlock)
	{
		ExperienceNumber_TextBlock->SetText(ExperienceText);
	}

	if(ExperiencePerHour_TextBlock)
	{
		ExperiencePerHour_TextBlock->SetVisibility(ESlateVisibility::Collapsed); //TODO Create logic for XP per h
	}
}

void UObsidianOverlayExperienceInfo::DestroyExperienceInfo()
{
	RemoveFromParent();
}



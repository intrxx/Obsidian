// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/MainOverlay/Subwidgets/ObsidianOverlayExperienceInfo.h"

#include "CommonTextBlock.h"
#include "Components/SizeBox.h"

void UObsidianOverlayExperienceInfo::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UObsidianOverlayExperienceInfo::InitializeExperienceInfo(const float CurrentExperience, const float MaxExperience, const float LastMaxExperience)
{
	const FText ExperienceText = FText::FromString(FString::Printf(TEXT("%d of %d"), FMath::TruncToInt(CurrentExperience), FMath::TruncToInt(MaxExperience)));
	if(ExperienceNumber_TextBlock)
	{
		ExperienceNumber_TextBlock->SetText(ExperienceText);
	}

	int32 Percentage = 0;
	if(MaxExperience > 0.0f)
	{
		Percentage = FMath::TruncToInt(((CurrentExperience - LastMaxExperience) / (MaxExperience - LastMaxExperience) * 100));
	}
	
	const FText ExperiencePercentageText = FText::FromString(FString::Printf(TEXT("%d%% to the next level."), Percentage));
	if(ExperiencePercentage_TextBlock)
	{
		ExperiencePercentage_TextBlock->SetText(ExperiencePercentageText);
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



// Copyright 2026 out of sCope team - intrxx

#include "UI/MainOverlay/SkillPoints/ObsidianSkillPointsNotification.h"

#include "CommonTextBlock.h"
#include "Components/Button.h"

// ~ Core

// ~ Project

void UObsidianSkillPointsNotification::NativeConstruct()
{
	Super::NativeConstruct();

	if(OpenTree_Button)
	{
		OpenTree_Button->OnClicked.AddDynamic(this, &ThisClass::OnPressed);
	}
}

void UObsidianSkillPointsNotification::OnPressed()
{
	checkf(OnSkillPointsNotificationPressedDelegate.IsBound(), TEXT("OpenTree_Button OnClicked delegate should be bound."));
	OnSkillPointsNotificationPressedDelegate.Broadcast();
}

void UObsidianSkillPointsNotification::SetSkillPointsCount(const float NewPoints) const
{
	if(SkillPointsCounter_TextBlock)
	{
		const FText	SkillPointsCountText = FText::AsNumber(FMath::TruncToInt(NewPoints));
		SkillPointsCounter_TextBlock->SetText(SkillPointsCountText);
	}
}

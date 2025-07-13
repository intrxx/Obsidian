// Copyright 2024 out of sCope team - Michał Ogiński

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
	checkf(OnSkillPointsNotificationPressed.IsBound(), TEXT("OpenTree_Button OnClicked delegate should be bound."));
	OnSkillPointsNotificationPressed.Broadcast();
}

void UObsidianSkillPointsNotification::SetSkillPointsCount(const float NewPoints) const
{
	if(SkillPointsCounter_TextBlock)
	{
		const FText	SkillPointsCountText = FText::FromString(FString::Printf(TEXT("%d"), FMath::TruncToInt(NewPoints)));
		SkillPointsCounter_TextBlock->SetText(SkillPointsCountText);
	}
}

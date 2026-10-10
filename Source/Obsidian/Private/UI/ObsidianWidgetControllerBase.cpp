// Copyright 2026 out of sCope team - intrxx

#include "UI/ObsidianWidgetControllerBase.h"

#include "Characters/Player/ObsidianLocalPlayer.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"


void UObsidianHeroWidgetControllerBase::SetWidgetControllerParams(
	const FObsidianWidgetControllerParams& InWidgetControllerParams)
{
	OwnerPlayerController = InWidgetControllerParams.ObsidianPlayerController.Get();
	OwnerLocalPlayer = InWidgetControllerParams.ObsidianLocalPlayer.Get();
	OwnerPlayerState = InWidgetControllerParams.ObsidianPlayerState.Get();
}

void UObsidianHeroWidgetControllerBase::OnWidgetControllerSetupCompleted()
{
}

void UObsidianHeroWidgetControllerBase::HandleBindingCallbacks(UObsidianAbilitySystemComponent* InObsidianASC)
{
}


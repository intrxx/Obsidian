// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/ObsidianWidgetControllerBase.h"

#include "Characters/Player/ObsidianLocalPlayer.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"

void UObsidianHeroWidgetControllerBase::SetWidgetControllerParams(
	const FObsidianWidgetControllerParams& WidgetControllerParams)
{
	OwnerPlayerController = WidgetControllerParams.ObsidianPlayerController.Get();
	OwnerLocalPlayer = WidgetControllerParams.ObsidianLocalPlayer.Get();
	OwnerPlayerState = WidgetControllerParams.ObsidianPlayerState.Get();
}

void UObsidianHeroWidgetControllerBase::OnWidgetControllerSetupCompleted()
{
}

void UObsidianHeroWidgetControllerBase::HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC)
{
}


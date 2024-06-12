// Copyright 2024 out of sCope team - Michał Ogiński

#include "Characters/Player/ObsidianPlayerController.h"
#include "UI/ObsidianHUD.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Player/ObsidianPlayerState.h"

AObsidianPlayerController::AObsidianPlayerController()
{
	bReplicates = true;
}

void AObsidianPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	//SetMouseCursorWidget(EMouseCursor::Default, DefaultCursor);
	CurrentMouseCursor = EMouseCursor::Default;
	
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AObsidianPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if(UObsidianAbilitySystemComponent* ObsidianASC = GetObsidianAbilitySystemComponent())
	{
		ObsidianASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
}

AObsidianPlayerState* AObsidianPlayerController::GetObsidianPlayerState() const
{
	return CastChecked<AObsidianPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UObsidianAbilitySystemComponent* AObsidianPlayerController::GetObsidianAbilitySystemComponent() const
{
	const AObsidianPlayerState* ObsidianPS = GetObsidianPlayerState();
	return (ObsidianPS ? ObsidianPS->GetObsidianAbilitySystemComponent() : nullptr);
}

AObsidianHUD* AObsidianPlayerController::GetObsidianHUD() const
{
	return GetHUD<AObsidianHUD>();
}

void AObsidianPlayerController::SetupHeroHealthBarWidget()
{
}




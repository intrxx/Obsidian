// Copyright 2024 Michał Ogiński


#include "CharacterComponents/ObsidianHeroComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/Player/ObsidianLocalPlayer.h"
#include "Input/OEnhancedInputUserSettings.h"
#include "InputMappingContext.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "Characters/ObsidianPawnData.h"
#include "GameFramework/PlayerController.h"
#include "Input/ObsidianEnhancedInputComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"

UObsidianHeroComponent::UObsidianHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UObsidianHeroComponent::InitializePlayerInput(UInputComponent* InputComponent)
{
	check(InputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if(Pawn == nullptr)
	{
		return;
	}

	APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const UObsidianLocalPlayer* LP = Cast<UObsidianLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(EnhancedInputSubsystem);

	EnhancedInputSubsystem->ClearAllMappings();

	const UObsidianPawnExtensionComponent* PawnExtComp = UObsidianPawnExtensionComponent::FindPawnExtComponent(Pawn);
	check(PawnExtComp);

	if(const UObsidianPawnData* PawnData = PawnExtComp->GetPawnData())
	{
		if(const UObsidianInputConfig* InputConfig = PawnData->InputConfig)
		{
			for(const FInputMappingContextAndPriority& Mapping : PawnData->DefaultMappingContext)
			{
				if(UInputMappingContext* IMC = Mapping.InputMapping)
				{
					if(Mapping.bRegisterWithSettings)
					{
						if(UEnhancedInputUserSettings* Settings = EnhancedInputSubsystem->GetUserSettings())
						{
							Settings->RegisterInputMappingContext(IMC);
						}

						FModifyContextOptions Options = {};
						Options.bIgnoreAllPressedKeysUntilRelease = false;

						EnhancedInputSubsystem->AddMappingContext(IMC, Mapping.Priority, Options);
					}
				}
			}
			
			UObsidianEnhancedInputComponent* ObsidianInputComponent = Cast<UObsidianEnhancedInputComponent>(InputComponent);
			if(ensureMsgf(ObsidianInputComponent, TEXT("Unexpected Input Component")))
			{
				TArray<uint32> BindHandles;
				// TODO Bind ability input here

				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_Move,
					ETriggerEvent::Triggered,this, &ThisClass::Input_Move, true);
			}
		}
	}
	
	PC->bShowMouseCursor;
	PC->CurrentMouseCursor = EMouseCursor::Hand;
	
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	PC->SetInputMode(InputModeData);
}

void UObsidianHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;
	
	if(Controller)
	{
		const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if(InputAxisVector.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, InputAxisVector.X);
		}

		if(InputAxisVector.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			 Pawn->AddMovementInput(MovementDirection, InputAxisVector.Y);
		}
	}
}
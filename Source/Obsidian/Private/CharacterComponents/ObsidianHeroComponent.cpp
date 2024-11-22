// Copyright 2024 out of sCope team - Michał Ogiński


#include "CharacterComponents/ObsidianHeroComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/Player/ObsidianLocalPlayer.h"
#include "Input/OEnhancedInputUserSettings.h"
#include "InputMappingContext.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "Characters/ObsidianPawnData.h"
#include "Interaction/ObsidianHighlightInterface.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PlayerController.h"
#include "Input/ObsidianEnhancedInputComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/ObsidianHUD.h"

UObsidianHeroComponent::UObsidianHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
	AutoRunSplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("AutoRunSplineComponent"));
}

void UObsidianHeroComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CursorTrace();
	AutoRun();
}

void UObsidianHeroComponent::AutoRun()
{
	if(!bAutoRunning)
	{
		return;
	}
	
	if(APawn* Pawn = GetPawn<APawn>())
	{
		const FVector LocationOnSpline = AutoRunSplineComp->FindLocationClosestToWorldLocation(Pawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = AutoRunSplineComp->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		Pawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if(DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void UObsidianHeroComponent::CursorTrace()
{
	APlayerController* PC = GetController<APlayerController>();
	if(PC == nullptr)
	{
		return;
	}
	
	PC->GetHitResultUnderCursor(Obsidian_TraceChannel_PlayerCursorTrace, false, CursorHit);

	if(!CursorHit.bBlockingHit)
	{
		return;
	}

	LastHighlightedActor = CurrentHighlightedActor;
	CurrentHighlightedActor = Cast<IObsidianHighlightInterface>(CursorHit.GetActor());

	if(LastHighlightedActor != CurrentHighlightedActor)
	{
		if(LastHighlightedActor)
		{
			LastHighlightedActor->StopHighlight();

			AActor* TargetActor = LastHighlightedActor->GetHighlightAvatarActor();
			if(TargetActor->ActorHasTag(ObsidianActorTags::RegularEnemy))
			{
				if(AObsidianPlayerController* OPC = Cast<AObsidianPlayerController>(PC))
				{
					OPC->UpdateHoveredRegularEnemyTarget(nullptr, false);
				}
			}
		}

		if(CurrentHighlightedActor)
		{
			CurrentHighlightedActor->StartHighlight();
			
			AActor* TargetActor = CurrentHighlightedActor->GetHighlightAvatarActor();
			if(TargetActor->ActorHasTag(ObsidianActorTags::RegularEnemy))
			{
				if(AObsidianPlayerController* OPC = Cast<AObsidianPlayerController>(PC))
				{
					OPC->UpdateHoveredRegularEnemyTarget(TargetActor, true);
				}
			}
		}
	}
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
				ObsidianInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed,
					&ThisClass::Input_AbilityInputTagReleased, /*OUT*/ BindHandles);

				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_Move_Keyboard,
					ETriggerEvent::Triggered,this, &ThisClass::Input_MoveKeyboard, true);
				
				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_Move_Mouse,
					ETriggerEvent::Started, this, &ThisClass::Input_MoveStartedMouse, false);
				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_Move_Mouse,
					ETriggerEvent::Triggered, this, &ThisClass::Input_MoveTriggeredMouse, false);
				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_Move_Mouse,
					ETriggerEvent::Completed, this, &ThisClass::Input_MoveReleasedMouse, false);
				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_Move_Mouse,
					ETriggerEvent::Canceled, this, &ThisClass::Input_MoveReleasedMouse, false);
				
				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_CharacterStatus,
					ETriggerEvent::Triggered, this, &ThisClass::Input_ToggleCharacterStatus, false);
				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_Inventory,
					ETriggerEvent::Triggered, this, &ThisClass::Input_ToggleInventory, false);
				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_PassiveSkillTree,
					ETriggerEvent::Triggered, this, &ThisClass::Input_TogglePassiveSkillTree, false);
			}
		}
	}
}

void UObsidianHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if(const APawn* Pawn = GetPawn<APawn>())
	{
		if(const UObsidianPawnExtensionComponent* PawnExtComp = UObsidianPawnExtensionComponent::FindPawnExtComponent(Pawn))
		{
			if(UObsidianAbilitySystemComponent* ObsidianASC = PawnExtComp->GetObsidianAbilitySystemComponent())
			{
				ObsidianASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void UObsidianHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if(const APawn* Pawn = GetPawn<APawn>())
	{
		if(const UObsidianPawnExtensionComponent* PawnExtComp = UObsidianPawnExtensionComponent::FindPawnExtComponent(Pawn))
		{
			if(UObsidianAbilitySystemComponent* ObsidianASC = PawnExtComp->GetObsidianAbilitySystemComponent())
			{
				ObsidianASC->AbilityInputTagReleased(InputTag);
			}
		}
	}
}

void UObsidianHeroComponent::Input_MoveKeyboard(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;
	
	if(Controller)
	{
		bAutoRunning = false;
		
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

void UObsidianHeroComponent::Input_MoveStartedMouse()
{
	bAutoRunning = false;
}

void UObsidianHeroComponent::Input_MoveTriggeredMouse()
{
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	if(CursorHit.bBlockingHit)
	{
		CachedDestination = CursorHit.Location;
	}
	
	if(APawn* Pawn = GetPawn<APawn>())
	{
		const FVector WorldDirection = (CachedDestination - Pawn->GetActorLocation()).GetSafeNormal();
		Pawn->AddMovementInput(WorldDirection);
	}
}

void UObsidianHeroComponent::Input_MoveReleasedMouse()
{
	APlayerController* PC = GetController<APlayerController>();
	if(PC == nullptr)
	{
		return;
	}
	
	APawn* Pawn = GetPawn<APawn>();
	if(FollowTime <= ShortPressThreshold && Pawn)
	{
		UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToLocationSynchronously(this,
			Pawn->GetActorLocation(), CachedDestination);
		if(NavigationPath)
		{
			AutoRunSplineComp->ClearSplinePoints();
			for(const FVector& PointLocation : NavigationPath->PathPoints)
			{
				AutoRunSplineComp->AddSplinePoint(PointLocation, ESplineCoordinateSpace::World);
			}
			
			if(!NavigationPath->PathPoints.IsEmpty())
			{
				CachedDestination = NavigationPath->PathPoints[NavigationPath->PathPoints.Num() - 1];
			}
			bAutoRunning = true;
		}
	}
	FollowTime = 0.f;
}

void UObsidianHeroComponent::Input_ToggleCharacterStatus()
{
	if(const AObsidianHUD* ObsidianHUD = GetObsidianHUD())
	{
		ObsidianHUD->ToggleCharacterStatus();
	}
}

void UObsidianHeroComponent::Input_ToggleInventory()
{
	if(const AObsidianHUD* ObsidianHUD = GetObsidianHUD())
	{
		ObsidianHUD->ToggleInventory();
	}
}

void UObsidianHeroComponent::Input_TogglePassiveSkillTree()
{
	if(const AObsidianHUD* ObsidianHUD = GetObsidianHUD())
	{
		ObsidianHUD->TogglePassiveSkillTree();
	}
}

AObsidianHUD* UObsidianHeroComponent::GetObsidianHUD() const
{
	if(const AObsidianPlayerController* ObsidianPC = GetController<AObsidianPlayerController>())
	{
		return ObsidianPC->GetObsidianHUD();
	}
	return nullptr;
}

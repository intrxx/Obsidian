// Copyright 2026 out of sCope team - intrxx

#include "CharacterComponents/ObsidianPlayerInputManager.h"

#include <EnhancedInputSubsystems.h>
#include <InputMappingContext.h>
#include <NavigationPath.h>
#include <NavigationSystem.h>
#include <Components/SplineComponent.h>
#include <GameFramework/PlayerController.h>
#include <CommonUIExtensions.h>

#include "Characters/Player/ObsidianLocalPlayer.h"
#include "Input/OEnhancedInputUserSettings.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "Characters/ObsidianPawnData.h"
#include "Interaction/ObsidianHighlightInterface.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Core/FunctionLibraries/ObsidianBlueprintFunctionLibrary.h"
#include "InventoryItems/Items/ObsidianDroppableItem.h"
#include "Input/ObsidianEnhancedInputComponent.h"
#include "Interaction/ObsidianInteractionInterface.h"
#include "InventoryItems/ObsidianItemManagerComponent.h"
#include "InventoryItems/Crafting/ObsidianCraftingComponent.h"
#include "InventoryItems/ItemLabelSystem/ObsidianItemLabelManagerSubsystem.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/ObsidianHUD.h"

DEFINE_LOG_CATEGORY(LogInteraction);

UObsidianPlayerInputManager::UObsidianPlayerInputManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
	AutoRunSplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("AutoRunSplineComponent"));
}

void UObsidianPlayerInputManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CursorTrace();
	AutoRun();
}

void UObsidianPlayerInputManager::AutoRun()
{
	if(!bAutoRunning)
	{
		if(bWasAutoMovingLastTick)
		{
			const APawn* Pawn = GetPawn<APawn>();
			if(Pawn && Pawn->GetVelocity().IsNearlyZero())
			{
				if(bAutoRunToPickupItemByLabel)
				{
					OnArrivedAtAcceptableItemPickupRange.Broadcast();
					bAutoRunToPickupItemByLabel = false;
				}
				if(bAutoRunToInteract)
				{
					OnArrivedAtAcceptableInteractionRange.Broadcast();
					bAutoRunToInteract = false;
				}
				
				bWasAutoMovingLastTick = false;
			}
		}
		return;
	}
	
	if(APawn* Pawn = GetPawn<APawn>())
	{
		const FVector LocationOnSpline = AutoRunSplineComp->FindLocationClosestToWorldLocation(Pawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if(DistanceToDestination <= AutoRunAcceptanceRadius)
		{
#if 0 // https://github.com/intrxx/Obsidian/commit/e3eda3899a1b39ec1952221a24bce0b40b7be769
			if(CanDropItem())
			{
				ServerHandleDroppingItem();
			}
#endif
			bAutoRunning = false;
			return;
		}
		
		const FVector Direction = AutoRunSplineComp->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		Pawn->AddMovementInput(Direction);
		bWasAutoMovingLastTick = true;
	}
}

void UObsidianPlayerInputManager::CursorTrace()
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

AObsidianHUD* UObsidianPlayerInputManager::GetObsidianHUD() const
{
	if(const AObsidianPlayerController* ObsidianPC = GetController<AObsidianPlayerController>())
	{
		return ObsidianPC->GetObsidianHUD();
	}
	return nullptr;
}

void UObsidianPlayerInputManager::InitializePlayerInput(UInputComponent* InputComponent)
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

				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_DropItem,
					ETriggerEvent::Triggered, this, &ThisClass::Input_DropItem, false);
				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_ReleaseUsingItem,
					ETriggerEvent::Triggered, this, &ThisClass::Input_ReleaseUsingItem, false);
				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_ReleaseContinouslyUsingItem,
					ETriggerEvent::Completed, this, &ThisClass::Input_ReleaseUsingItem, false);
				
				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_Interact,
					ETriggerEvent::Triggered, this, &ThisClass::Input_Interact, false);

				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_WeaponSwap,
					ETriggerEvent::Triggered, this, &ThisClass::Input_WeaponSwap, false);

				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_UI_OpenGameplayMenu,
					ETriggerEvent::Triggered, this, &ThisClass::Input_OpenGameplayMenu, false);

				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_UI_ToggleHighlight,
					ETriggerEvent::Triggered, this, &ThisClass::Input_ToggleHighlight, false);
			}
		}
	}

	OwnerCraftingComponent = PC->FindComponentByClass<UObsidianCraftingComponent>();
	check(OwnerCraftingComponent);
	OwnerItemManagerComponent = PC->FindComponentByClass<UObsidianItemManagerComponent>();
	check(OwnerItemManagerComponent);
}

void UObsidianPlayerInputManager::Input_AbilityInputTagPressed(FGameplayTag InputTag)
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

void UObsidianPlayerInputManager::Input_AbilityInputTagReleased(FGameplayTag InputTag)
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

void UObsidianPlayerInputManager::Input_MoveKeyboard(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if(Pawn == nullptr)
	{
		return;
	}
	
	if(const AController* Controller = Pawn->GetController())
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

		StopOngoingInteraction();
	}
}

void UObsidianPlayerInputManager::Input_MoveStartedMouse()
{
	if(CanMoveMouse() == false)
	{
		return;
	}

	UObsidianCraftingComponent* CraftingComp = OwnerCraftingComponent.Get();
	if(CraftingComp && CraftingComp->IsUsingItem())
	{
		CraftingComp->SetUsingItem(false);
	}

	if(ActiveInteractionTarget && ActiveInteractionTarget->RequiresOngoingInteraction())
	{
		ActiveInteractionTarget->StopInteraction(GetController<AObsidianPlayerController>());
		ActiveInteractionTarget = nullptr;

#if !UE_BUILD_SHIPPING
		if(bDebugInteraction)
		{
			UE_LOG(LogInteraction, Display, TEXT("Stopped ongoing interaction."))	
		}
#endif
	}
	
	bAutoRunning = false;
}

void UObsidianPlayerInputManager::Input_MoveTriggeredMouse()
{
	if(CanContinuouslyMoveMouse() == false)
	{
		return;
	}
	
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

void UObsidianPlayerInputManager::Input_MoveReleasedMouse()
{
	bWantsToInteract = false;
	
	if(CanMoveMouse() == false)
	{
		// If we just dropped item clicking in the world, reset the JustDroppedItem variable so we can move again.
		if(OwnerItemManagerComponent && OwnerItemManagerComponent->DidJustDroppedItem()) 
		{
			OwnerItemManagerComponent->SetJustDroppedItem(false);
		}
		return;
	}
	
	APlayerController* PC = GetController<APlayerController>();
	if(PC == nullptr)
	{
		return;
	}
	
	if(FollowTime <= ShortPressThreshold)
	{
		AutoRunToClickedLocation();
	}
	FollowTime = 0.f;
}

void UObsidianPlayerInputManager::AutoRunToClickedLocation()
{
	const APawn* Pawn = GetPawn<APawn>();
	if(Pawn == nullptr)
	{
		return;
	}
	
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

void UObsidianPlayerInputManager::Input_ToggleCharacterStatus()
{
	if(const AObsidianHUD* ObsidianHUD = GetObsidianHUD())
	{
		ObsidianHUD->ToggleCharacterStatus();
	}
}

void UObsidianPlayerInputManager::Input_ToggleInventory()
{
	if(const AObsidianHUD* ObsidianHUD = GetObsidianHUD())
	{
		ObsidianHUD->ToggleInventory();
	}
}

void UObsidianPlayerInputManager::Input_TogglePassiveSkillTree()
{
	if(const AObsidianHUD* ObsidianHUD = GetObsidianHUD())
	{
		ObsidianHUD->TogglePassiveSkillTree();
	}
}

void UObsidianPlayerInputManager::Input_DropItem()
{
	if (OwnerItemManagerComponent)
	{
		OwnerItemManagerComponent->DropItem();
	}
}

void UObsidianPlayerInputManager::Input_ReleaseUsingItem()
{
	UObsidianCraftingComponent* CraftingComp = OwnerCraftingComponent.Get();
	if(CraftingComp && CraftingComp->IsUsingItem())
	{
		CraftingComp->SetUsingItem(false);
	}
}

void UObsidianPlayerInputManager::Input_Interact()
{
	if(CursorHit.bBlockingHit == false)
	{
		return;
	}

	AActor* InteractionActor = CursorHit.GetActor();
	if(InteractionActor == nullptr)
	{
		return;
	}

	if(IObsidianInteractionInterface* InteractionTarget = Cast<IObsidianInteractionInterface>(InteractionActor))
	{
		if(InteractionTarget->CanInteract() == false)
		{
#if !UE_BUILD_SHIPPING
			if(bDebugInteraction)
			{
				UE_LOG(LogInteraction, Display, TEXT("Target cannot be interacted with."))	
			}
#endif
			return;
		}

		StopOngoingInteraction();
		
		bWantsToInteract = true;
		ActiveInteractionTarget = InteractionActor;
		ServerStartInteraction(ActiveInteractionTarget);
#if !UE_BUILD_SHIPPING
		if(bDebugInteraction)
		{
			UE_LOG(LogInteraction, Display, TEXT("Starting Interaction."))	
		}
#endif
	}
}

void UObsidianPlayerInputManager::Input_WeaponSwap()
{
	if (OwnerItemManagerComponent)
	{
		OwnerItemManagerComponent->ServerWeaponSwap();
	}
}

void UObsidianPlayerInputManager::Input_OpenGameplayMenu()
{
	if (GameplayMenuClass.IsNull())
	{
		return;
	}
	
	const APawn* Pawn = GetPawn<APawn>();
	if (Pawn == nullptr)
	{
		return;
	}
	
	const AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(Pawn->GetController());
	if (ObsidianPC == nullptr)
	{
		return;
	}

	const ULocalPlayer* LocalPlayer = ObsidianPC->GetLocalPlayer();
	if (LocalPlayer == nullptr)
	{
		return;
	}

	//TODO(intrxx) Close all Gameplay Menus First
	// This actually need some more work, Gameplay UI like Inventory, Stash etc. should be handled with Common UI first
	
	UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(LocalPlayer, ObsidianGameplayTags::UI_Layer_GameplayMenu,
		GameplayMenuClass);
}

void UObsidianPlayerInputManager::Input_ToggleHighlight()
{
	static bool bHighlight = true;
	if (const UWorld* World = GetWorld())
	{
		if (UObsidianItemLabelManagerSubsystem* ItemLabelManager = World->GetSubsystem<UObsidianItemLabelManagerSubsystem>())
		{
			//TODO(intrxx) Use some global highlight setting
			bHighlight = !bHighlight;
			ItemLabelManager->ToggleItemLabelHighlight(bHighlight);
		}
	}
}

bool UObsidianPlayerInputManager::HandlePickUpIfItemOutOfRange(AObsidianDroppableItem* ItemToPickUp, const EObsidianItemPickUpType PickUpType)
{
	const AActor* OwnerActor = GetOwner();
	if(OwnerActor == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwnerActor is null in [%hs]"), __FUNCTION__);
		return false;
	}

	if(OwnerActor->HasAuthority() == false)
	{
		UE_LOG(LogInventory, Error, TEXT("[%hs] should not be called without authority."), __FUNCTION__);
		return false;
	}

	const FVector ItemLocation = ItemToPickUp->GetActorLocation();
	const FVector OwnerLocation = OwnerActor->GetActorLocation();
	
	const float DistanceToItem = FVector::Dist2D(FVector(OwnerLocation.X, OwnerLocation.Y, 0.0f), FVector(ItemLocation.X, ItemLocation.Y, 0.0f)); 
	if (DistanceToItem > DefaultInteractionRadius + AutoRunAcceptanceRadius)
	{
		const FVector ApproachDestination = ItemLocation - ((ItemLocation - OwnerLocation).GetSafeNormal()) * DefaultInteractionRadius;
		ClientStartApproachingOutOfRangeItem(ApproachDestination, ItemToPickUp, PickUpType);
		return true;
	}
	return false;
}

bool UObsidianPlayerInputManager::HandleOutOfRangeInteraction(const TScriptInterface<IObsidianInteractionInterface>& InteractionTarget, const FVector& TargetLocation)
{
	const AActor* OwnerActor = GetOwner();
	if(OwnerActor == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwnerActor is null in [%hs]"), __FUNCTION__);
		return false;
	}

	if(OwnerActor->HasAuthority() == false)
	{
		UE_LOG(LogInventory, Error, TEXT("[%hs] should not be called without authority."), __FUNCTION__);
		return false;
	}

	float InteractionRadius = InteractionTarget->GetInteractionRadius();
	InteractionRadius = InteractionRadius == 0.0f ? DefaultInteractionRadius : InteractionRadius;

	const FVector OwnerLocation = OwnerActor->GetActorLocation();
	const float DistanceToTarget = FVector::Dist2D(FVector(OwnerLocation.X, OwnerLocation.Y, 0.0f), FVector(TargetLocation.X, TargetLocation.Y, 0.0f));
	
#if !UE_BUILD_SHIPPING
	if(bDebugInteraction)
	{
		UE_LOG(LogInteraction, Display, TEXT("Calculating Distance to Interaction Target in [%hs]"), __FUNCTION__);

		if(const UWorld* World = GetWorld())
		{
			UObsidianBlueprintFunctionLibrary::PrintVector3D(World, TargetLocation, TEXT("(Red Sphere) Target Location: "));
			UE_LOG(LogInteraction, Display, TEXT("Interaction Radius: [%f]."), InteractionRadius);
			DrawDebugSphere(World, TargetLocation, InteractionRadius, 16, FColor::Red, false, 5.0f);

			UObsidianBlueprintFunctionLibrary::PrintVector3D(World, OwnerLocation, TEXT("(Blue Sphere) Player Location on distance check: "));
			DrawDebugSphere(World, OwnerLocation, 32.0f, 8, FColor::Blue, false, 5.0f);

			UE_LOG(LogInteraction, Display, TEXT("Calculated Distance: [%f]."), DistanceToTarget);
		}
	}
#endif
	
	if(DistanceToTarget > InteractionRadius + AutoRunAcceptanceRadius)
	{
		const FVector ApproachDestination = TargetLocation - ((TargetLocation - OwnerLocation).GetSafeNormal()) * InteractionRadius;

#if !UE_BUILD_SHIPPING
		if(bDebugInteraction)
		{
			UE_LOG(LogInteraction, Display, TEXT("Player out of Interacting range. Calculating Approach Destination."));

			if(const UWorld* World = GetWorld())
			{
				UObsidianBlueprintFunctionLibrary::PrintVector3D(World, ApproachDestination, TEXT("(Yellow Sphere) Approach Destination: "));
				DrawDebugSphere(World, ApproachDestination, 32.0f, 8, FColor::Yellow, false, 5.0f);
			}
		}
#endif
		
		ClientStartApproachingOutOfRangeInteractionTarget(ApproachDestination);
		return true;
	}
	return false;
}

void UObsidianPlayerInputManager::ClientStartApproachingOutOfRangeInteractionTarget_Implementation(const FVector_NetQuantize10& ToDestination)
{
	bAutoRunToInteract = true;
	CachedDestination = ToDestination;
	
	if(OnArrivedAtAcceptableInteractionRange.IsBound())
	{
		OnArrivedAtAcceptableInteractionRange.Clear();
	}

	OnArrivedAtAcceptableInteractionRange.AddUObject(this, &ThisClass::InteractWithOutOfRangeTarget);
	
	AutoRunToClickedLocation();
}

void UObsidianPlayerInputManager::ServerStartInteraction_Implementation(const TScriptInterface<IObsidianInteractionInterface>& InteractionTarget)
{
	if(InteractionTarget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("InteractionTarget is null in [%hs]"), __FUNCTION__);
		return;
	}

	const AActor* InteractionActor = InteractionTarget->GetInteractionActor();
	if(InteractionActor == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("InteractionActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const FVector InteractionLocation = InteractionActor->GetActorLocation();
	if(HandleOutOfRangeInteraction(InteractionTarget, InteractionLocation))
	{
#if !UE_BUILD_SHIPPING
		if(bDebugInteraction)
		{
			UE_LOG(LogInteraction, Display, TEXT("Interaction target is out of range. Approaching."))	
		}
#endif
		return;
	}

	//TODO(intrxx) Perform validation here, if there is no validation, there is no point of keeping this client -> server -> client logic
	// HandleOutOfRangeInteraction is some kind of validating

	if(InteractionTarget->CanInteract())
	{
		ClientTriggerInteraction(InteractionTarget);
	}
}

void UObsidianPlayerInputManager::InteractWithOutOfRangeTarget()
{
	if(ActiveInteractionTarget)
	{
#if !UE_BUILD_SHIPPING
		if(bDebugInteraction)
		{
			UE_LOG(LogInteraction, Display, TEXT("Trying to interact with out of range Target after approaching."));
			if(const UWorld* World = GetWorld())
			{
				if(const AActor* Owner = GetOwner())
				{
					const FVector OwnerLocation = Owner->GetActorLocation();
					UObsidianBlueprintFunctionLibrary::PrintVector3D(World, OwnerLocation, TEXT("(Green Sphere) Player Location after Approaching: "));
					DrawDebugSphere(World, OwnerLocation, 32.0f, 8, FColor::Green, false, 5.0f);
				}
			}
		}
#endif
		ServerStartInteraction(ActiveInteractionTarget);
		
		OnArrivedAtAcceptableInteractionRange.Clear();
	}
}

void UObsidianPlayerInputManager::ClientTriggerInteraction_Implementation(const TScriptInterface<IObsidianInteractionInterface>& InteractionTarget)
{
	if(InteractionTarget)
	{
#if !UE_BUILD_SHIPPING
		if(bDebugInteraction)
		{
			UE_LOG(LogInteraction, Display, TEXT("Interacting."))	
		}
#endif
		InteractionTarget->Interact(GetController<AObsidianPlayerController>());

		if(ActiveInteractionTarget->RequiresOngoingInteraction() == false)
		{
			ActiveInteractionTarget = nullptr;
		}
#if !UE_BUILD_SHIPPING
		else
		{
			if(bDebugInteraction)
			{
				UE_LOG(LogInteraction, Display, TEXT("Target requires ongoing interaction."))	
			}
		}
#endif
	}
}

void UObsidianPlayerInputManager::ClientStartApproachingOutOfRangeItem_Implementation(const FVector_NetQuantize10& ToDestination, AObsidianDroppableItem* ItemToPickUp, const EObsidianItemPickUpType PickUpType)
{
	bAutoRunToPickupItemByLabel = true;
	CachedDestination = ToDestination;
	CachedDroppableItemToPickup = ItemToPickUp;
	
	if(OnArrivedAtAcceptableItemPickupRange.IsBound())
	{
		OnArrivedAtAcceptableItemPickupRange.Clear();
	}

	if(PickUpType == EObsidianItemPickUpType::AutomaticPickUp)
	{
		OnArrivedAtAcceptableItemPickupRange.AddUObject(this, &ThisClass::AutomaticallyPickupOutOfRangeItem);
	}
	else if(PickUpType == EObsidianItemPickUpType::PickUpToDrag)
	{
		OnArrivedAtAcceptableItemPickupRange.AddUObject(this, &ThisClass::DragOutOfRangeItem);
	}
	
	AutoRunToClickedLocation();
}

void UObsidianPlayerInputManager::AutomaticallyPickupOutOfRangeItem()
{
	if(CachedDroppableItemToPickup)
	{
		if (OwnerItemManagerComponent)
		{
			OwnerItemManagerComponent->ServerPickupItem(CachedDroppableItemToPickup);
		}
		
		OnArrivedAtAcceptableItemPickupRange.Clear();
		CachedDroppableItemToPickup = nullptr;
	}
}

void UObsidianPlayerInputManager::DragOutOfRangeItem()
{
	if(CachedDroppableItemToPickup)
	{
		if (OwnerItemManagerComponent)
		{
			OwnerItemManagerComponent->ServerGrabDroppableItemToCursor(CachedDroppableItemToPickup);
		}
		
		OnArrivedAtAcceptableItemPickupRange.Clear();
		CachedDroppableItemToPickup = nullptr;
	}
}

bool UObsidianPlayerInputManager::IsHoveringOverInteractionTarget() const
{
	if(CursorHit.bBlockingHit)
	{
		const AActor* InteractionActor = CursorHit.GetActor();
		if(InteractionActor && Cast<IObsidianInteractionInterface>(InteractionActor))
		{
			return true;
		}
	}
	return false;
}

void UObsidianPlayerInputManager::StopOngoingInteraction()
{
	if(ActiveInteractionTarget && ActiveInteractionTarget->RequiresOngoingInteraction())
	{
		ActiveInteractionTarget->StopInteraction(GetController<AObsidianPlayerController>());
		ActiveInteractionTarget = nullptr;

#if !UE_BUILD_SHIPPING
		if(bDebugInteraction)
		{
			UE_LOG(LogInteraction, Display, TEXT("Stopped ongoing interaction."))	
		}
#endif
	}
	else
	{
#if !UE_BUILD_SHIPPING
		if(bDebugInteraction)
		{
			UE_LOG(LogInteraction, Display, TEXT("Was unable to stop the ongoing interaction."))	
		}
#endif
	}
}

bool UObsidianPlayerInputManager::CanMoveMouse() const
{
	if (OwnerItemManagerComponent)
	{
		return !OwnerItemManagerComponent->CanDropItem()
					&& !OwnerItemManagerComponent->DidJustDroppedItem()
					&& !IsHoveringOverInteractionTarget();
	}
	return !IsHoveringOverInteractionTarget();
}

bool UObsidianPlayerInputManager::CanContinuouslyMoveMouse() const
{
	if (OwnerItemManagerComponent)
	{
		return !OwnerItemManagerComponent->CanDropItem()
					&& !OwnerItemManagerComponent->DidJustDroppedItem()
					&& !bWantsToInteract;
	}
	return !bWantsToInteract;
}



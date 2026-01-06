// Copyright 2024 out of sCope team - Michał Ogiński

#include "CharacterComponents/ObsidianPlayerInputManager.h"

// ~ Core
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "NavigationPath.h"
#include "Engine/ActorChannel.h"
#include "NavigationSystem.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"

// ~ Project
#include "CommonUIExtensions.h"
#include "Characters/Player/ObsidianLocalPlayer.h"
#include "Input/OEnhancedInputUserSettings.h"
#include "UI/Inventory/Items/ObsidianDraggedItem_Simple.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "Characters/ObsidianPawnData.h"
#include "Interaction/ObsidianHighlightInterface.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Core/FunctionLibraries/ObsidianBlueprintFunctionLibrary.h"
#include "InventoryItems/Items/ObsidianDroppableItem.h"
#include "Input/ObsidianEnhancedInputComponent.h"
#include "Interaction/ObsidianInteractionInterface.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "Obsidian/ObsidianGameModule.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/ObsidianHUD.h"
#include "UI/Inventory/Items/ObsidianDraggedItem.h"
#include "UI/Inventory/Items/ObsidianItem.h"

DEFINE_LOG_CATEGORY(LogInteraction);

UObsidianPlayerInputManager::UObsidianPlayerInputManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
	AutoRunSplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("AutoRunSplineComponent"));
}

void UObsidianPlayerInputManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, DraggedItem, COND_OwnerOnly);
}

void UObsidianPlayerInputManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CursorTrace();
	AutoRun();

	if(bDraggingItem)
	{
		DragItem();
	}

	if(bUsingItem)
	{
		DragUsableItemIcon();
	}
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

void UObsidianPlayerInputManager::DragItem() const
{
	const APlayerController* PC = GetController<APlayerController>();
	if(PC == nullptr)
	{
		return;
	}

	float LocationX = 0.0f;
	float LocationY = 0.0f;
	if(DraggedItemWidget && PC->GetMousePosition(LocationX, LocationY))
	{
		const FVector2D ViewportPosition = FVector2D(LocationX, LocationY);
		DraggedItemWidget->SetPositionInViewport(ViewportPosition);
	}
}
   
void UObsidianPlayerInputManager::DragUsableItemIcon() const
{
	const APlayerController* PC = GetController<APlayerController>();
	if(PC == nullptr)
	{
		return;
	}

	float LocationX = 0.0f;
	float LocationY = 0.0f;
	if(DraggedUsableItemWidget && PC->GetMousePosition(LocationX, LocationY))
	{
		if(UWorld* World = GetWorld())
		{
			const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(World);
			FVector2D ItemSize = DraggedUsableItemWidget->GetItemWidgetSize() + FVector2D(5.0f, -5.0f);
			ItemSize *= DPIScale;
			
			const FVector2D ViewportPosition = FVector2D(LocationX - ItemSize.X, LocationY - ItemSize.Y);
			DraggedUsableItemWidget->SetPositionInViewport(ViewportPosition);
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
			}
		}
	}
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

	if(IsUsingItem())
	{
		SetUsingItem(false);
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
		if(bJustDroppedItem) // If we just dropped item clicking in the world, reset the JustDroppedItem variable so we can move again.
		{
			bJustDroppedItem = false;
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
	if(CanDropItem() == false)
	{
		return;
	}
	ServerHandleDroppingItem();
}

void UObsidianPlayerInputManager::Input_ReleaseUsingItem()
{
	if(IsUsingItem())
	{
		SetUsingItem(false);
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
	ServerWeaponSwap();
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

void UObsidianPlayerInputManager::SetUsingItem(const bool InbUsingItem, UObsidianItem* ItemWidget,
	UObsidianInventoryItemInstance* UsingInstance)
{
	if(InbUsingItem && ItemWidget)
	{
		if (UsingInstance == nullptr)
		{
			UE_LOG(LogObsidian, Error, TEXT("UsingInstance is invalid in [%hs]."), __FUNCTION__);
			return;
		}
		
		UWorld* World = GetWorld();
		if(World == nullptr)
		{
			return;
		}
		
		if(DraggedUsableItemWidget)
		{
			DraggedUsableItemWidget->RemoveFromParent();
		}

		ItemWidget->SetUsingItemProperties();
		CachedUsingInventoryItemWidget = ItemWidget;

		checkf(DraggedUsableItemWidgetClass, TEXT("DraggedUsableItemWidgetClass is invalid in [%hs] please fill it."),
			__FUNCTION__);
		DraggedUsableItemWidget = CreateWidget<UObsidianDraggedItem_Simple>(World, DraggedUsableItemWidgetClass);
		DraggedUsableItemWidget->InitializeDraggedItem(ItemWidget->GetItemImage(), UsingInstance->GetItemGridSpan());
		DraggedUsableItemWidget->AddToViewport();

		UsingItemInstance = UsingInstance;
	}
	else
	{
		if(DraggedUsableItemWidget)
		{
			DraggedUsableItemWidget->RemoveFromParent();
		}

		if(CachedUsingInventoryItemWidget)
		{
			CachedUsingInventoryItemWidget->ResetUsingItemProperties();
		}

		DraggedUsableItemWidget = nullptr;
		UsingItemInstance = nullptr;

		OnStopUsingItemDelegate.Broadcast();
	}

	bUsingItem = InbUsingItem;
}

bool UObsidianPlayerInputManager::DropItem()
{
	if(CanDropItem() == false)
	{
		return false;
	}
	ServerHandleDroppingItem();
	return true;
}

void UObsidianPlayerInputManager::ServerGrabStashedItemToCursor_Implementation(const FObsidianItemPosition& FromPosition)
{
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if(PlayerStashComponent == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("PlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = PlayerStashComponent->GetItemInstanceFromTabAtPosition(FromPosition);
	if(InstanceToGrab == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("InstanceToGrab is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	if(PlayerStashComponent->RemoveItemInstance(InstanceToGrab) == false)
	{
		return;
	}
	
	DraggedItem = FDraggedItem(InstanceToGrab);
	StartDraggingItem(Controller);
}

void UObsidianPlayerInputManager::ServerTransferItemToInventory_Implementation(const FObsidianItemPosition& FromStashPosition)
{
	const AController* Controller = GetController<AController>();
	if (Controller == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if (PlayerStashComponent == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("PlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if (InventoryComponent == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = PlayerStashComponent->GetItemInstanceFromTabAtPosition(FromStashPosition);
	if (InstanceToGrab == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("InstanceToGrab is null in [%hs]"), __FUNCTION__);
		return;
	}

	if (InventoryComponent->CanFitItemInstance(InstanceToGrab))
	{
		if (PlayerStashComponent->RemoveItemInstance(InstanceToGrab))
		{
			InventoryComponent->AddItemInstance(InstanceToGrab);
		}
	}
}

void UObsidianPlayerInputManager::ServerReplaceItemAtStashPosition_Implementation(const FObsidianItemPosition& AtStashPosition)
{
	const AController* Controller = GetController<AController>();
	if (Controller == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if (PlayerStashComponent == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("PlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	if (PlayerStashComponent->CanOwnerModifyPlayerStashState() == false)
	{
		return;
	}
	
	const FDraggedItem CachedDraggedItem = DraggedItem;
	DraggedItem.Clear();
	StopDraggingItem(Controller);
	
	ServerGrabStashedItemToCursor(AtStashPosition);

	bool bSuccess = false;
	if (UObsidianInventoryItemInstance* Instance = CachedDraggedItem.Instance)
	{
		bSuccess = PlayerStashComponent->AddItemInstanceToSpecificSlot(Instance, AtStashPosition);
	}
	else if (const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = CachedDraggedItem.ItemDef)
	{
		bSuccess = PlayerStashComponent->AddItemDefinitionToSpecifiedSlot(ItemDef, AtStashPosition, CachedDraggedItem.GeneratedData);
	}
	
	if (bSuccess == false)
	{
		ServerAddItemToStashTabAtSlot(AtStashPosition, false);
		DraggedItem = CachedDraggedItem;
		StartDraggingItem(Controller);
	}
}

void UObsidianPlayerInputManager::ServerAddStacksFromDraggedItemToStashedItemAtSlot_Implementation(const FObsidianItemPosition& AtPosition, const int32 StacksToAddOverride)
{
	if(DraggedItem.IsEmpty())
	{
		UE_LOG(LogPlayerStash, Error, TEXT("Tried to add Stacks from Dragged Item, but Dragged Item is empty [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if(PlayerStashComponent == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("PlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianInventoryItemInstance* Instance = DraggedItem.Instance;
	if(Instance && Instance->IsStackable())
	{
		const int32 CurrentStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		const FObsidianAddingStacksResult AddingStacksResult = PlayerStashComponent->TryAddingStacksToSpecificSlotWithInstance(Instance, AtPosition, StacksToAddOverride);
		
		UpdateDraggedItem(AddingStacksResult, CurrentStackCount, Controller);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
		if(DefaultObject && DefaultObject->IsStackable())
		{
			const int32 CurrentStackCount = DraggedItem.GeneratedData.AvailableStackCount;
			const FObsidianAddingStacksResult AddingStacksResult = PlayerStashComponent->TryAddingStacksToSpecificSlotWithItemDef(ItemDef, CurrentStackCount, AtPosition, StacksToAddOverride);
			
			UpdateDraggedItem(AddingStacksResult, CurrentStackCount, Controller);
		}
	}
}

void UObsidianPlayerInputManager::ServerTakeoutFromStashedItem_Implementation(const FObsidianItemPosition& AtStashPosition, const int32 StacksToTake)
{
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if(PlayerStashComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("PlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* ItemInstance = PlayerStashComponent->GetItemInstanceFromTabAtPosition(AtStashPosition);
	if(ItemInstance == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("ItemInstance is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const FObsidianItemOperationResult Result = PlayerStashComponent->TakeOutFromItemInstance(ItemInstance, StacksToTake);
	const UObsidianInventoryItemInstance* AffectedInstance = Result.AffectedInstance;
	if(AffectedInstance == nullptr || Result.bActionSuccessful == false)
	{
		return;
	}

	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = AffectedInstance->GetItemDef();
	if(ItemDef == nullptr)
	{
		return;
	}

	//TODO(intrxx) In this case this are actually StacksToTake, maybe create another struct to reflect that?
	DraggedItem = FDraggedItem(ItemDef, Result.StacksLeft);
	StartDraggingItem(Controller);

	if(ItemDef && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(ItemDef);
	}
}

void UObsidianPlayerInputManager::ServerActivateUsableItemFromStash_Implementation(UObsidianInventoryItemInstance* UsingInstance)
{
	if(UsingInstance == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("UsingInstance is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if(PlayerStashComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	PlayerStashComponent->UseItem(UsingInstance, nullptr);
}

bool UObsidianPlayerInputManager::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething =  Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	UObsidianInventoryItemInstance* Instance = DraggedItem.Instance;
	if(Instance && IsValid(Instance))
	{
		WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
	}
	
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef;
	if(ItemDef && IsValid(ItemDef))
	{
		WroteSomething |= Channel->ReplicateSubobject(ItemDef, *Bunch, *RepFlags);
	}

	if(UsingItemInstance && IsValid(UsingItemInstance))
	{
		WroteSomething |= Channel->ReplicateSubobject(UsingItemInstance, *Bunch, *RepFlags);
	}

	return WroteSomething;
}

void UObsidianPlayerInputManager::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing UObsidianInventoryItemInstance
	if(IsUsingRegisteredSubObjectList() && !DraggedItem.IsEmpty())
	{
		UObsidianInventoryItemInstance* Instance = DraggedItem.Instance;
		if(IsValid(Instance))
		{
			AddReplicatedSubObject(Instance);
		}

		const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef;
		if(IsValid(ItemDef))
		{
			AddReplicatedSubObject(ItemDef);
		}

		if(IsValid(UsingItemInstance))
		{
			AddReplicatedSubObject(UsingItemInstance);
		}
	}
}

void UObsidianPlayerInputManager::OnRep_DraggedItem(const FDraggedItem& OldDraggedItem)
{
	if(DraggedItem.IsEmpty() && bDraggingItem) // We cleared Dragged Item, so we should no longer drag it
	{
		const AController* Controller = GetController<AController>();
		if(Controller == nullptr)
		{
			UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
			return;
		}
		StopDraggingItem(Controller);
	}
	else if(!DraggedItem.IsEmpty() && !bDraggingItem || DraggedItemWasReplaced(OldDraggedItem))  // We got new Item to drag
	{
		const AController* Controller = GetController<AController>();
		if(Controller == nullptr)
		{
			UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
			return;
		}
		StartDraggingItem(Controller);
	}
	else if(DraggedItem.GeneratedData.AvailableStackCount > 0) // We are dragging an item but the stacks changed. //TODO(intrxx) Why just "Stacks > 0"?
	{
		if(DraggedItemWidget)
		{
			DraggedItemWidget->UpdateStackCount(DraggedItem.GeneratedData.AvailableStackCount);
		}
	}
	//TODO(intrxx) I don't think I need to account for Rarity and Affixes changes but check in later 
}

void UObsidianPlayerInputManager::ServerTakeoutFromInventoryItem_Implementation(const FIntPoint& ItemGridPosition, const int32 StacksToTake)
{
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* ItemInstance = InventoryComponent->GetItemInstanceAtLocation(ItemGridPosition);
	if(ItemInstance == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("ItemInstance is null in [%hs]"), __FUNCTION__);
		return;
	}

	const FObsidianItemOperationResult Result = InventoryComponent->TakeOutFromItemInstance(ItemInstance, StacksToTake);
	const UObsidianInventoryItemInstance* AffectedInstance = Result.AffectedInstance;
	if(AffectedInstance == nullptr || Result.bActionSuccessful == false)
	{
		return;
	}

	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = AffectedInstance->GetItemDef();
	if(ItemDef == nullptr)
	{
		return;
	}

	//TODO(intrxx) In this case this are actually StacksToTake, maybe create another struct to reflect that?
	DraggedItem = FDraggedItem(ItemDef, Result.StacksLeft);
	StartDraggingItem(Controller);

	if(ItemDef && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(ItemDef);
	}
}

void UObsidianPlayerInputManager::ServerReplaceItemAtInventorySlot_Implementation(const FIntPoint& ItemGridPosition)
{
	const AController* Controller = GetController<AController>();
	if (Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if (InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	if (InventoryComponent->CanOwnerModifyInventoryState() == false)
	{
		return;
	}
	
	const FDraggedItem CachedDraggedItem = DraggedItem;
	DraggedItem.Clear();
	StopDraggingItem(Controller);
	
	ServerGrabInventoryItemToCursor(ItemGridPosition);

	bool bSuccess = false;
	if (UObsidianInventoryItemInstance* Instance = CachedDraggedItem.Instance)
	{
		bSuccess = InventoryComponent->AddItemInstanceToSpecificSlot(Instance, ItemGridPosition);
	}
	else if (const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = CachedDraggedItem.ItemDef)
	{
		bSuccess = InventoryComponent->AddItemDefinitionToSpecifiedSlot(ItemDef, ItemGridPosition, CachedDraggedItem.GeneratedData);
	}
	
	if (bSuccess == false)
	{
		ServerAddItemToInventoryAtSlot(ItemGridPosition, false);
		DraggedItem = CachedDraggedItem;
		StartDraggingItem(Controller);
	}
}

void UObsidianPlayerInputManager::ServerReplaceItemAtEquipmentSlot_Implementation(const FGameplayTag& SlotTag, const FGameplayTag& EquipSlotTagOverride)
{
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
	if(EquipmentComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	if(EquipmentComponent->CanOwnerModifyEquipmentState() == false)
	{
		return;
	}

	const FDraggedItem CachedDraggedItem = DraggedItem;		
	DraggedItem.Clear();
	StopDraggingItem(Controller);
	
	ServerGrabEquippedItemToCursor(SlotTag);

	bool bSuccess = false;
	if(UObsidianInventoryItemInstance* Instance = CachedDraggedItem.Instance)
	{
		bSuccess = EquipmentComponent->ReplaceItemAtSpecificSlot(Instance, SlotTag, EquipSlotTagOverride);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = CachedDraggedItem.ItemDef)
	{
		bSuccess = EquipmentComponent->ReplaceItemAtSpecificSlot(ItemDef, SlotTag, CachedDraggedItem.GeneratedData, EquipSlotTagOverride);
	}
	
	if(bSuccess == false)
	{
		ServerEquipItemAtSlot(SlotTag);
		DraggedItem = CachedDraggedItem;
		StartDraggingItem(Controller);
	}
}

void UObsidianPlayerInputManager::ServerWeaponSwap_Implementation()
{
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
	if(EquipmentComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	EquipmentComponent->WeaponSwap();
}

void UObsidianPlayerInputManager::ServerAddItemToStashTabAtSlot_Implementation(const FObsidianItemPosition& AtPosition, const bool bShiftDown)
{
	if(DraggedItem.IsEmpty())
	{
		UE_LOG(LogPlayerStash, Error, TEXT("Tried to add Inventory Item to the Inventory at specific slot but the Dragged Item is Empty in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if(PlayerStashComponent == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("PlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const int32 StacksToAddOverride = bShiftDown ? 1 : -1;
	
	if(UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		const int32 CurrentStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		const FObsidianItemOperationResult Result = PlayerStashComponent->AddItemInstanceToSpecificSlot(Instance, AtPosition, StacksToAddOverride);
		
		UpdateDraggedItem(Result, CurrentStackCount, Controller);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		const int32 CachedStacks = DraggedItem.GeneratedData.AvailableStackCount;
		const FObsidianItemOperationResult Result = PlayerStashComponent->AddItemDefinitionToSpecifiedSlot(ItemDef, AtPosition, DraggedItem.GeneratedData, StacksToAddOverride);

		UpdateDraggedItem(Result, CachedStacks, Controller);
	}
}

void UObsidianPlayerInputManager::ServerPickupItem_Implementation(AObsidianDroppableItem* ItemToPickup)
{
	if(ItemToPickup == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("ItemToPickup is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Controller is null in [%hs]"), __FUNCTION__);
		return;
	}

	if(HandlePickUpIfItemOutOfRange(ItemToPickup, EObsidianItemPickUpType::AutomaticPickUp))
	{
		return;
	}
	
	const FObsidianPickupTemplate Template = ItemToPickup->GetPickupTemplateFromPickupContent();
	if(Template.IsValid())
	{
		const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = Template.ItemDef;
		if(ItemDef == nullptr)
		{
			UE_LOG(LogInventory, Error, TEXT("ItemDef is null in [%hs]"), __FUNCTION__);
			return;
		}

		if(const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject())
		{
			if(DefaultObject->IsEquippable())
			{
				UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
				if(EquipmentComponent == nullptr)
				{
					UE_LOG(LogInventory, Error, TEXT("EquipmentComponent is null in [%hs]"), __FUNCTION__);
					return;
				}
			
				if(EquipmentComponent->AutomaticallyEquipItem(ItemDef, Template.ItemGeneratedData))
				{
					ItemToPickup->DestroyDroppedItem();
					return;
				}
			}
		}

		UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
		if(InventoryComponent == nullptr)
		{
			UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
			return;
		}
		
		if(const FObsidianItemOperationResult& Result = InventoryComponent->AddItemDefinition(ItemDef, Template.ItemGeneratedData))
		{
			ItemToPickup->UpdateDroppedItemStacks(Result.StacksLeft);
		}
		
		return;
	}

	const FObsidianPickupInstance Instance = ItemToPickup->GetPickupInstanceFromPickupContent();
	if(Instance.IsValid())
	{
		UObsidianInventoryItemInstance* ItemInstance = Instance.Item;
		if(ItemInstance == nullptr)
		{
			UE_LOG(LogInventory, Error, TEXT("ItemInstance is null in [%hs]"), __FUNCTION__);
			return;
		}

		if(ItemInstance->IsItemEquippable())
		{
			UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
			if(EquipmentComponent == nullptr)
			{
				UE_LOG(LogInventory, Error, TEXT("EquipmentComponent is null in [%hs]"), __FUNCTION__);
				return;
			}
			
			if(EquipmentComponent->AutomaticallyEquipItem(ItemInstance))
			{
				ItemToPickup->DestroyDroppedItem();
				return;
			}
		}

		UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
		if(InventoryComponent == nullptr)
		{
			UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
			return;
		}


		const int32 CurrentStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		const FObsidianItemOperationResult Result = InventoryComponent->AddItemInstance(ItemInstance);
		if(CurrentStacks != Result.StacksLeft)
		{
			ItemToPickup->UpdateDroppedItemStacks(Result.StacksLeft);
		}
		return;
	}
}

void UObsidianPlayerInputManager::ServerHandleDroppingItem_Implementation()
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}

	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	AActor* Hero = GetOwner();
	if (Hero == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Hero is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const FVector OwnerLocation = Hero->GetActorLocation();

#if 0 // https://github.com/intrxx/Obsidian/commit/e3eda3899a1b39ec1952221a24bce0b40b7be769
	const FVector ClickedLocation = CursorHit.Location;
	const float ItemDropLocation = CachedItemDropLocation == FVector::Zero() ? FVector::Distance(OwnerLocation, ClickedLocation) : FVector::Distance(OwnerLocation, CachedItemDropLocation);
	if(false) // TODO Check if there is no room in the drop space for dropping the item, then move character to some other location at cached destination
	{
		if(CursorHit.bBlockingHit)
		{
			CachedDestination = ClickedLocation;
		}
		CachedItemDropLocation = CachedDestination;
		AutoRunToClickedLocation();
		return;
	}
	
	CachedItemDropLocation = FVector::Zero();
#endif
	
	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(World);
	if(NavigationSystem == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("NavigationSystem is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	//TODO(intrxx) Bias towards Actors forward Vector
	FNavLocation RandomPointLocation;
	NavigationSystem->GetRandomPointInNavigableRadius(OwnerLocation, DropRadius, RandomPointLocation);
	
	FHitResult GroundTraceResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Hero);
	const FVector GroundTraceEndLocation = FVector(RandomPointLocation.Location.X, RandomPointLocation.Location.Y, RandomPointLocation.Location.Z - 300.0f);
	World->LineTraceSingleByChannel(GroundTraceResult, RandomPointLocation.Location, GroundTraceEndLocation, ECC_Visibility, QueryParams);

	FRotator ItemRotation = FRotator::ZeroRotator;
	FVector ItemLocation = RandomPointLocation.Location;
	if(GroundTraceResult.bBlockingHit) // We are able to align the item to the ground better
	{
		FVector RandomisedRotationVector = FMath::VRand().GetSafeNormal();
		ItemRotation = UKismetMathLibrary::MakeRotFromZY(GroundTraceResult.ImpactNormal, RandomisedRotationVector);
		ItemLocation = GroundTraceResult.Location;
	}
	
	const FTransform ItemSpawnTransform = FTransform(ItemRotation, ItemLocation, FVector(1.0f, 1.0f, 1.0f));
	AObsidianDroppableItem* Item = World->SpawnActorDeferred<AObsidianDroppableItem>(AObsidianDroppableItem::StaticClass(), ItemSpawnTransform);
	Item->InitializeItem(DraggedItem);
	Item->FinishSpawning(ItemSpawnTransform);

	DraggedItem.Clear();

	StopDraggingItem(Controller);
}

void UObsidianPlayerInputManager::ServerGrabDroppableItemToCursor_Implementation(AObsidianDroppableItem* ItemToPickup)
{
	if(ItemToPickup == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("ItemToPickup is null in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	if(HandlePickUpIfItemOutOfRange(ItemToPickup, EObsidianItemPickUpType::PickUpToDrag))
	{
		return;
	}
	
	const FObsidianPickupTemplate Template = ItemToPickup->GetPickupTemplateFromPickupContent();
	if(Template.IsValid()) // We are grabbing Item Template
	{
		DraggedItem = FDraggedItem(Template.ItemDef, Template.ItemGeneratedData);
		ItemToPickup->UpdateDroppedItemStacks(0);
		
		StartDraggingItem(Controller);
		return;
	}

	const FObsidianPickupInstance Instance = ItemToPickup->GetPickupInstanceFromPickupContent();
	if(Instance.IsValid()) // We are grabbing Item Instance
	{
		DraggedItem = FDraggedItem(Instance.Item);
		ItemToPickup->UpdateDroppedItemStacks(0);
		
		StartDraggingItem(Controller);
		return;
	}

	checkf(false, TEXT("Provided ItemToPickup has no Instance nor Template to pick up, this is bad and should not happen."))
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
		ServerPickupItem(CachedDroppableItemToPickup);
		
		OnArrivedAtAcceptableItemPickupRange.Clear();
		CachedDroppableItemToPickup = nullptr;
	}
}

void UObsidianPlayerInputManager::DragOutOfRangeItem()
{
	if(CachedDroppableItemToPickup)
	{
		ServerGrabDroppableItemToCursor(CachedDroppableItemToPickup);
		
		OnArrivedAtAcceptableItemPickupRange.Clear();
		CachedDroppableItemToPickup = nullptr;
	}
}

void UObsidianPlayerInputManager::ServerGrabInventoryItemToCursor_Implementation(const FIntPoint& ItemGridPosition)
{
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = InventoryComponent->GetItemInstanceAtLocation(ItemGridPosition);
	if(InstanceToGrab == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InstanceToGrab is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	if(InventoryComponent->RemoveItemInstance(InstanceToGrab) == false)
	{
		return;
	}
	
	DraggedItem = FDraggedItem(InstanceToGrab);

	StartDraggingItem(Controller);
}

void UObsidianPlayerInputManager::ServerEquipItemAtSlot_Implementation(const FGameplayTag& SlotTag)
{
	if(DraggedItem.IsEmpty())
	{
		UE_LOG(LogEquipment, Error, TEXT("Tried to add Inventory Item to the Inventory at specific slot but the Dragged Item is Empty in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
	if(EquipmentComponent == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("EquipmentComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	if(UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		if(EquipmentComponent->EquipItemToSpecificSlot(Instance, SlotTag))
		{
			DraggedItem.Clear();
			StopDraggingItem(Controller);
		}
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		if(EquipmentComponent->EquipItemToSpecificSlot(ItemDef, SlotTag, DraggedItem.GeneratedData))
		{
			DraggedItem.Clear();
			StopDraggingItem(Controller);
		}
	}
}

void UObsidianPlayerInputManager::ServerGrabEquippedItemToCursor_Implementation(const FGameplayTag& SlotTag)
{
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
	if(EquipmentComponent == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = EquipmentComponent->GetEquippedInstanceAtSlot(SlotTag);
	if(InstanceToGrab == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("InstanceToGrab is null in [%hs]"), __FUNCTION__);
		return;
	}

	EquipmentComponent->UnequipItem(InstanceToGrab);
	
	DraggedItem = FDraggedItem(InstanceToGrab);

	StartDraggingItem(Controller);
}

void UObsidianPlayerInputManager::ServerAddItemToInventoryAtSlot_Implementation(const FIntPoint& AtGridSlot, const bool bShiftDown)
{
	if(DraggedItem.IsEmpty())
	{
		UE_LOG(LogInventory, Error, TEXT("Tried to add Inventory Item to the Inventory at specific slot but the Dragged Item is Empty in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const int32 StacksToAddOverride = bShiftDown ? 1 : -1;
	
	if(UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		const int32 CurrentStackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		const FObsidianItemOperationResult Result = InventoryComponent->AddItemInstanceToSpecificSlot(Instance, AtGridSlot, StacksToAddOverride);
		
		UpdateDraggedItem(Result, CurrentStackCount, Controller);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		const int32 CachedStacks = DraggedItem.GeneratedData.AvailableStackCount;
		const FObsidianItemOperationResult Result = InventoryComponent->AddItemDefinitionToSpecifiedSlot(ItemDef, AtGridSlot, DraggedItem.GeneratedData, StacksToAddOverride);

		UpdateDraggedItem(Result, CachedStacks, Controller);
	}
}

void UObsidianPlayerInputManager::ServerAddStacksFromDraggedItemToInventoryItemAtSlot_Implementation(const FIntPoint& ItemGridPosition, const int32 StacksToAddOverride)
{
	if(DraggedItem.IsEmpty())
	{
		UE_LOG(LogInventory, Error, TEXT("Tried to add Inventory Item to the Inventory at specific slot but the Dragged Item is Empty in [%hs]"), __FUNCTION__);
		return;
	}

	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianInventoryItemInstance* Instance = DraggedItem.Instance;
	if(Instance && Instance->IsStackable())
	{
		const int32 PreviousStacks = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		const FObsidianAddingStacksResult AddingStacksResult = InventoryComponent->TryAddingStacksToSpecificSlotWithInstance(Instance, ItemGridPosition, StacksToAddOverride);
		
		UpdateDraggedItem(AddingStacksResult, PreviousStacks, Controller);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
		if(DefaultObject && DefaultObject->IsStackable())
		{
			const int32 CachedStacks = DraggedItem.GeneratedData.AvailableStackCount;
			const FObsidianAddingStacksResult AddingStacksResult = InventoryComponent->TryAddingStacksToSpecificSlotWithItemDef(ItemDef, CachedStacks, ItemGridPosition, StacksToAddOverride);

			UpdateDraggedItem(AddingStacksResult, CachedStacks, Controller);
		}
	}
}

void UObsidianPlayerInputManager::UseItem(const FObsidianItemPosition& OnPosition, const bool bLeftShiftDown)
{
	ServerUseItem(UsingItemInstance, OnPosition);

	if(bLeftShiftDown == false)
	{
		SetUsingItem(false);
	}
}

void UObsidianPlayerInputManager::ServerTransferItemToPlayerStash_Implementation(const FIntPoint& FromInventoryPosition, const FGameplayTag& ToStashTab)
{
	const AController* Controller = GetController<AController>();
	if (Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if (PlayerStashComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("PlayerStashComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if (InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = InventoryComponent->GetItemInstanceAtLocation(FromInventoryPosition);
	if (InstanceToGrab == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InstanceToGrab is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	if (PlayerStashComponent->CanFitInstanceInStashTab(InstanceToGrab->GetItemGridSpan(), InstanceToGrab->GetItemCategoryTag(), InstanceToGrab->GetItemBaseTypeTag(), ToStashTab))
	{
		if (InventoryComponent->RemoveItemInstance(InstanceToGrab))
		{
			PlayerStashComponent->AddItemInstance(InstanceToGrab, ToStashTab);
		}
	}
}

void UObsidianPlayerInputManager::ServerUseItem_Implementation(UObsidianInventoryItemInstance* UsingInstance, const FObsidianItemPosition& OnPosition)
{
	if(UsingInstance == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("UsingInstance is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	const FGameplayTag StashTabTag = OnPosition.GetOwningStashTabTag();
	if (StashTabTag != FGameplayTag::EmptyTag)
	{
		UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
		if(PlayerStashComponent == nullptr)
		{
			UE_LOG(LogPlayerStash, Error, TEXT("PlayerStashComponent is null in [%hs]"), __FUNCTION__);
			return;
		}

		UObsidianInventoryItemInstance* UsingOntoInstance = PlayerStashComponent->GetItemInstanceFromTabAtPosition(OnPosition);
		PlayerStashComponent->UseItem(UsingInstance, UsingOntoInstance);
	}
	else
	{
		UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
		if(InventoryComponent == nullptr)
		{
			UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
			return;
		}

		UObsidianInventoryItemInstance* UsingOntoInstance = InventoryComponent->GetItemInstanceAtLocation(OnPosition.GetItemGridPosition());
		InventoryComponent->UseItem(UsingInstance, UsingOntoInstance);
	}
}

void UObsidianPlayerInputManager::ServerActivateUsableItemFromInventory_Implementation(UObsidianInventoryItemInstance* UsingInstance)
{
	if(UsingInstance == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("UsingInstance is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	InventoryComponent->UseItem(UsingInstance, nullptr);
}

void UObsidianPlayerInputManager::UpdateStacksOnDraggedItemWidget(const int32 InStacks)
{
	if(DraggedItemWidget)
	{
		DraggedItemWidget->UpdateStackCount(InStacks);
	}
}

bool UObsidianPlayerInputManager::DraggedItemWasReplaced(const FDraggedItem& OldDraggedItem) const
{
	if(OldDraggedItem.Instance && OldDraggedItem.Instance != DraggedItem.Instance)
	{
		return true;
	}

	if(OldDraggedItem.ItemDef && DraggedItem.Instance)
	{
		return true;
	}
	
	return false;
}

void UObsidianPlayerInputManager::StartDraggingItem(const AController* Controller)
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}
	
	if(Controller && !Controller->IsLocalController())
	{
		return;
	}

	if(DraggedItemWidget)
	{
		DraggedItemWidget->RemoveFromParent();
	}
	
	checkf(DraggedItemWidgetClass, TEXT("DraggedItemWidgetClass is invalid in UObsidianPlayerInputManager::DragItem please fill it on ObsidianDroppableItem Instance."));
	UObsidianDraggedItem* Item = CreateWidget<UObsidianDraggedItem>(World, DraggedItemWidgetClass);

	bool bInitialized = false;
	if(UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		Item->InitializeItemWidgetWithItemInstance(Instance);
		bInitialized = true;
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		Item->InitializeItemWidgetWithItemDef(ItemDef, DraggedItem.GeneratedData);
		bInitialized = true;
	}
	checkf(bInitialized, TEXT("Item was not initialized with neither Instance nor ItemDef, this is bad and should not happen."));
	
	Item->AddToViewport();
	
	DraggedItemWidget = Item;
	bDraggingItem = true;

	TWeakObjectPtr<UObsidianPlayerInputManager> WeakThis(this);
	World->GetTimerManager().SetTimerForNextTick([WeakThis]()
	{
		if(WeakThis.IsValid())
		{
			WeakThis->bItemAvailableForDrop = true;
		}
	});

	OnStartDraggingItemDelegate.Broadcast(DraggedItem);
}

void UObsidianPlayerInputManager::StopDraggingItem(const AController* Controller)
{
	if(Controller && !Controller->IsLocalController())
	{
		return;
	}
	
	if(DraggedItemWidget)
	{
		DraggedItemWidget->RemoveFromParent();
	}
	
	bDraggingItem = false;
	DraggedItemWidget = nullptr;
	bItemAvailableForDrop = false;

	bJustDroppedItem = true;

	OnStopDraggingItemDelegate.Broadcast();
}

void UObsidianPlayerInputManager::UpdateDraggedItem(const FObsidianItemOperationResult& OperationResult, const int32 CachedNumberOfStack, const AController* ForController)
{
	if(CachedNumberOfStack != OperationResult.StacksLeft)
	{
		if(OperationResult.bActionSuccessful)
		{
			DraggedItem.Clear();
			StopDraggingItem(ForController);
			return;
		}
		UpdateStacksOnDraggedItemWidget(OperationResult.StacksLeft);
		DraggedItem.GeneratedData.AvailableStackCount = OperationResult.StacksLeft;
	}
}

void UObsidianPlayerInputManager::UpdateDraggedItem(const FObsidianAddingStacksResult& OperationResult, const int32 CachedNumberOfStack, const AController* ForController)
{
	if(CachedNumberOfStack != OperationResult.StacksLeft)
	{
		if(OperationResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_WholeItemAsStacksAdded)
		{
			DraggedItem.Clear();
			StopDraggingItem(ForController);
		}
		else if(OperationResult.AddingStacksResult == EObsidianAddingStacksResultType::ASR_SomeOfTheStacksAdded)
		{
			UpdateStacksOnDraggedItemWidget(OperationResult.StacksLeft);
			DraggedItem.GeneratedData.AvailableStackCount = OperationResult.StacksLeft;
		}
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
}

bool UObsidianPlayerInputManager::CanDropItem() const
{
	return IsDraggingAnItem() && bItemAvailableForDrop;
}

bool UObsidianPlayerInputManager::CanMoveMouse() const
{
	return !CanDropItem() && !bJustDroppedItem && !IsHoveringOverInteractionTarget();
}

bool UObsidianPlayerInputManager::CanContinuouslyMoveMouse() const
{
	return !CanDropItem() && !bJustDroppedItem && !bWantsToInteract;
}



// Copyright 2024 out of sCope team - Michał Ogiński

#include "CharacterComponents/ObsidianHeroComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/Player/ObsidianLocalPlayer.h"
#include "Input/OEnhancedInputUserSettings.h"
#include "InputMappingContext.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "Characters/ObsidianPawnData.h"
#include "Interaction/ObsidianHighlightInterface.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PlayerController.h"
#include "Gameplay/InventoryItems/ObsidianDroppableItem.h"
#include "Input/ObsidianEnhancedInputComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/ObsidianHUD.h"
#include "UI/Inventory/ObsidianDraggedItem.h"

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

	if(bDragItem)
	{
		DragItem();
	}
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

				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_DropItem,
					ETriggerEvent::Triggered, this, &ThisClass::Input_DropItem, false);
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
	}
}

void UObsidianHeroComponent::Input_MoveStartedMouse()
{
	if(CanMoveMouse() == false)
	{
		return;
	}
	
	bAutoRunning = false;
}

void UObsidianHeroComponent::Input_MoveTriggeredMouse()
{
	if(CanMoveMouse() == false)
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

void UObsidianHeroComponent::Input_MoveReleasedMouse()
{
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

void UObsidianHeroComponent::Input_DropItem()
{
	if(CanDropItem() == false)
	{
		return;
	}
	HandleDroppingItem();
}

bool UObsidianHeroComponent::DropItem()
{
	if(CanDropItem() == false)
	{
		return false;
	}
	return HandleDroppingItem();
}

void UObsidianHeroComponent::ServerPickupItemDef_Implementation(AObsidianDroppableItem* ItemToPickup)
{
	if(!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianHeroComponent::ServerPickupItemDef_Implementation."))
		return;
	}

	if(ItemToPickup == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("ItemToPickup is null in UObsidianHeroComponent::ServerPickupItemDef_Implementation."));
		return;
	}
	
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in UObsidianHeroComponent::ServerPickupItemDef_Implementation."));
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in UObsidianHeroComponent::ServerPickupItemDef_Implementation."));
		return;
	}

	const FPickupTemplate Template = ItemToPickup->GetPickupTemplateFromPickupContent();
	if(!Template.IsValid())
	{
		UE_LOG(LogInventory, Error, TEXT("Template is null in UObsidianHeroComponent::ServerPickupItemDef_Implementation."));
		return;
	}
	
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = Template.ItemDef;
	if(ItemDef == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("ItemDef is null in UObsidianHeroComponent::ServerPickupItemDef_Implementation."));
		return;
	}

	const int32 StackCount = Template.StackCount;
	int32 OutStacksLeft;
	InventoryComponent->AddItemDefinition(ItemDef, OutStacksLeft, StackCount);
	ItemToPickup->UpdateDroppedItemStacks(OutStacksLeft);
}


void UObsidianHeroComponent::ServerPickupItemInstance_Implementation(AObsidianDroppableItem* ItemToPickup)
{
	if(!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Authority in UObsidianHeroComponent::ServerPickupItemInstance_Implementation."))
		return;
	}

	if(ItemToPickup == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("ItemToPickup is null in UObsidianHeroComponent::ServerPickupItemInstance_Implementation."));
		return;
	}
	
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in UObsidianHeroComponent::ServerPickupItemInstance_Implementation."));
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in UObsidianHeroComponent::ServerPickupItemInstance_Implementation."));
		return;
	}

	const FPickupInstance Instance = ItemToPickup->GetPickupInstanceFromPickupContent();
	if(!Instance.IsValid())
	{
		UE_LOG(LogInventory, Error, TEXT("Instance is null in UObsidianHeroComponent::ServerPickupItemInstance_Implementation."));
		return;
	}

	UObsidianInventoryItemInstance* ItemInstance = Instance.Item;
	if(ItemInstance == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("ItemInstance is null in UObsidianHeroComponent::ServerPickupItemInstance_Implementation."));
		return;
	}

	int32 OutStacksLeft;
	InventoryComponent->AddItemInstance(ItemInstance, OutStacksLeft);
	if(OutStacksLeft > 0)
	{
		ItemInstance->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, OutStacksLeft);
	}
	ItemToPickup->UpdateDroppedItemStacks(OutStacksLeft);
}

bool UObsidianHeroComponent::HandleDroppingItem()
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return false;
	}
	
	const FVector CursorHitLocation = CursorHit.Location;
	const FTransform ItemSpawnTransform = FTransform(FRotator::ZeroRotator, CursorHitLocation, FVector(1.0f, 1.0f, 1.0f));
	AObsidianDroppableItem* Item = World->SpawnActorDeferred<AObsidianDroppableItem>(DroppableItemClass, ItemSpawnTransform);

	const UObsidianDraggedItem* CurrentlyDraggedItem = GetCurrentlyDraggedItem();
	if(UObsidianInventoryItemInstance* ItemInstance = CurrentlyDraggedItem->GetItemInstance())
	{
		Item->AddItemInstance(ItemInstance);
	}
	else if(TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = CurrentlyDraggedItem->GetItemDef())
	{
		const int32 Stacks = CurrentlyDraggedItem->GetItemStacks();
		Item->AddItemDefinition(ItemDef, Stacks);
	}
	Item->FinishSpawning(ItemSpawnTransform);
	StopDragging();

	bJustDroppedItem = true;
	return true;
}

AObsidianHUD* UObsidianHeroComponent::GetObsidianHUD() const
{
	if(const AObsidianPlayerController* ObsidianPC = GetController<AObsidianPlayerController>())
	{
		return ObsidianPC->GetObsidianHUD();
	}
	return nullptr;
}

void UObsidianHeroComponent::DragItem(UObsidianDraggedItem* InDraggedItem)
{
	const UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}
	
	DraggedItem = InDraggedItem;
	bDragItem = true;

	TWeakObjectPtr<UObsidianHeroComponent> WeakThis(this);
	World->GetTimerManager().SetTimerForNextTick([WeakThis]()
	{
		if(WeakThis.IsValid())
		{
			WeakThis->bItemAvailableForDrop = true;
		}
	});
}

void UObsidianHeroComponent::StopDragging()
{
	DraggedItem->RemoveFromParent();
	
	bDragItem = false;
	DraggedItem = nullptr;
	bItemAvailableForDrop = false;
}

void UObsidianHeroComponent::DragItem() const
{
	const APlayerController* PC = GetController<APlayerController>();
	if(PC == nullptr)
	{
		return;
	}

	float LocationX = 0.0f;
	float LocationY = 0.0f;
	if(PC->GetMousePosition(LocationX, LocationY))
	{
		const FVector2D ViewportPosition = FVector2D(LocationX, LocationY);
		DraggedItem->SetPositionInViewport(ViewportPosition);
	}
}

bool UObsidianHeroComponent::CanDropItem() const
{
	return !bCursorOverUI && IsDraggingAnItem() && bItemAvailableForDrop;
}

bool UObsidianHeroComponent::CanMoveMouse() const
{
	return !CanDropItem() && !bJustDroppedItem;
}



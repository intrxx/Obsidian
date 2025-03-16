// Copyright 2024 out of sCope team - Michał Ogiński

#include "CharacterComponents/ObsidianHeroComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/Player/ObsidianLocalPlayer.h"
#include "Input/OEnhancedInputUserSettings.h"
#include "InputMappingContext.h"
#include "NavigationPath.h"
#include "Engine/ActorChannel.h"
#include "NavigationSystem.h"
#include "UI/Inventory/ObsidianDraggedItem_Simple.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Blueprint/WidgetLayoutLibrary.h"
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
#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "Net/UnrealNetwork.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/ObsidianHUD.h"
#include "UI/Inventory/ObsidianDraggedItem.h"
#include "UI/Inventory/ObsidianItem.h"

UObsidianHeroComponent::UObsidianHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
	AutoRunSplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("AutoRunSplineComponent"));
}

void UObsidianHeroComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, DraggedItem, COND_OwnerOnly);
}

void UObsidianHeroComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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

void UObsidianHeroComponent::DragItem() const
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

void UObsidianHeroComponent::DragUsableItemIcon() const
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
			FVector2D ItemSize = DraggedUsableItemWidget->GetItemSize() + FVector2D(5.0f, -5.0f);
			ItemSize *= DPIScale;
			
			const FVector2D ViewportPosition = FVector2D(LocationX - ItemSize.X, LocationY - ItemSize.Y);
			DraggedUsableItemWidget->SetPositionInViewport(ViewportPosition);
		}
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
				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_ReleaseUsingItem,
					ETriggerEvent::Triggered, this, &ThisClass::Input_ReleaseUsingItem, false);
				ObsidianInputComponent->BindNativeAction(InputConfig, ObsidianGameplayTags::Input_ReleaseContinouslyUsingItem,
					ETriggerEvent::Completed, this, &ThisClass::Input_ReleaseUsingItem, false);
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

	if(IsUsingItem())
	{
		SetUsingItem(false);
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
	ServerHandleDroppingItem(CursorHit.Location);
}

void UObsidianHeroComponent::Input_ReleaseUsingItem()
{
	if(IsUsingItem())
	{
		SetUsingItem(false);
	}
}

void UObsidianHeroComponent::SetUsingItem(const bool InbUsingItem, UObsidianItem* ItemWidget, UObsidianInventoryItemInstance* UsingInstance)
{
	if(InbUsingItem && ItemWidget)
	{
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

		checkf(DraggedUsableItemWidgetClass, TEXT("DraggedUsableItemWidgetClass is invalid in UObsidianHeroComponent::SetUsingItem please fill it."));
		DraggedUsableItemWidget = CreateWidget<UObsidianDraggedItem_Simple>(World, DraggedUsableItemWidgetClass);
		DraggedUsableItemWidget->InitializeDraggedItem(ItemWidget->GetItemImage(), ItemWidget->GetItemGridSpan());
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

bool UObsidianHeroComponent::DropItem()
{
	if(CanDropItem() == false)
	{
		return false;
	}
	ServerHandleDroppingItem(CursorHit.Location);
	return true;
}

bool UObsidianHeroComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
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

void UObsidianHeroComponent::ReadyForReplication()
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

void UObsidianHeroComponent::OnRep_DraggedItem(const FDraggedItem& OldDraggedItem)
{
	if(DraggedItem.IsEmpty() && bDraggingItem) // We cleared Dragged Item, so we should no longer drag it
	{
		const AController* Controller = GetController<AController>();
		if(Controller == nullptr)
		{
			UE_LOG(LogInventory, Error, TEXT("OwningActor is null in UObsidianHeroComponent::OnRep_DraggedItem."));
			return;
		}
		StopDraggingItem(Controller);
	}
	else if(!DraggedItem.IsEmpty() && !bDraggingItem || DraggedItemWasReplaced(OldDraggedItem))  // We got new Item to drag
	{
		const AController* Controller = GetController<AController>();
		if(Controller == nullptr)
		{
			UE_LOG(LogInventory, Error, TEXT("OwningActor is null in UObsidianHeroComponent::OnRep_DraggedItem."));
			return;
		}
		StartDraggingItem(Controller);
	}
	else if(DraggedItem.Stacks > 0) // We are dragging an item but the stacks changed
	{
		if(DraggedItemWidget)
		{
			DraggedItemWidget->UpdateStackCount(DraggedItem.Stacks);
		}
	}
}

void UObsidianHeroComponent::ServerTakeoutFromItem_Implementation(const FVector2D& SlotPosition, const int32 StacksToTake)
{
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in UObsidianHeroComponent::ServerTakeoutFromItem_Implementation."));
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in UObsidianHeroComponent::ServerTakeoutFromItem_Implementation."));
		return;
	}

	UObsidianInventoryItemInstance* ItemInstance = InventoryComponent->GetItemInstanceAtLocation(SlotPosition);
	if(ItemInstance == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("ItemInstance is null in UObsidianHeroComponent::ServerTakeoutFromItem_Implementation."));
		return;
	}
	
	UObsidianInventoryItemInstance* NewInstance = InventoryComponent->TakeOutFromItemInstance(ItemInstance, StacksToTake);
	if(NewInstance == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Result of TakeOutFromItemInstance is null in UObsidianHeroComponent::ServerTakeoutFromItem_Implementation."));
		return;
	}
	
	DraggedItem = FDraggedItem(NewInstance);
	StartDraggingItem(Controller);

	if(NewInstance && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(NewInstance);
	}
}

void UObsidianHeroComponent::ServerReplaceItemAtInventorySlot_Implementation(const FVector2D& SlotPosition)
{
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in UObsidianHeroComponent::ServerReplaceItemAtInventorySlot_Implementation."));
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in UObsidianHeroComponent::ServerReplaceItemAtInventorySlot_Implementation."));
		return;
	}
	
	const FDraggedItem CachedDraggedItem = DraggedItem;
	DraggedItem.Clear();
	StopDraggingItem(Controller);
	
	ServerGrabInventoryItemToCursor(SlotPosition);

	bool bSuccess = false;
	if(UObsidianInventoryItemInstance* Instance = CachedDraggedItem.Instance)
	{
		bSuccess = InventoryComponent->AddItemInstanceToSpecificSlot(Instance, SlotPosition);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = CachedDraggedItem.ItemDef)
	{
		const int32 Stacks = CachedDraggedItem.Stacks;
		int32 StacksLeft = Stacks;
		if(InventoryComponent->AddItemDefinitionToSpecifiedSlot(ItemDef, SlotPosition, StacksLeft, Stacks))
		{
			bSuccess = true;
		}
	}
	
	// We fall back and add the previous item to the inventory again, this could happen If the Client cheated through its check to begin the replacement
	if(bSuccess == false)
	{
		UE_LOG(LogInventory, Error, TEXT("Client cheated the check to replace the item! Discarding the replacement."))
		ServerAddItemToInventoryAtSlot(SlotPosition, false);
	}
}

void UObsidianHeroComponent::ServerReplaceItemAtEquipmentSlotSlot_Implementation(const FGameplayTag& SlotTag)
{
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in UObsidianHeroComponent::ServerReplaceItemAtEquipmentSlotSlot_Implementation."));
		return;
	}

	UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
	if(EquipmentComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in UObsidianHeroComponent::ServerReplaceItemAtEquipmentSlotSlot_Implementation."));
		return;
	}

	const FDraggedItem CachedDraggedItem = DraggedItem;
	DraggedItem.Clear();
	StopDraggingItem(Controller);
	
	ServerGrabEquippedItemToCursor(SlotTag);

	bool bSuccess = false;
	if(UObsidianInventoryItemInstance* Instance = CachedDraggedItem.Instance)
	{
		bSuccess = EquipmentComponent->EquipItemToSpecificSlot(Instance, SlotTag);
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = CachedDraggedItem.ItemDef)
	{
		if(EquipmentComponent->EquipItemToSpecificSlot(ItemDef, SlotTag))
		{
			bSuccess = true;
		}
	}

	// We fall back and equip the previously equipped item again, this could happen If the Client cheated through its check to begin the replacement
	if(bSuccess == false)
	{
		UE_LOG(LogEquipment, Error, TEXT("Client cheated the check to replace the equipped item! Discarding the replacement."))
		ServerEquipItemAtSlot(SlotTag);	
	}
}

void UObsidianHeroComponent::ServerPickupItemDef_Implementation(AObsidianDroppableItem* ItemToPickup)
{
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

	if(const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject())
	{
		if(DefaultObject->IsEquippable())
		{
			UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
			if(EquipmentComponent == nullptr)
			{
				UE_LOG(LogInventory, Error, TEXT("EquipmentComponent is null in UObsidianHeroComponent::ServerPickupItemDef_Implementation."));
				return;
			}
			
			if(EquipmentComponent->AutomaticallyEquipItem(ItemDef))
			{
				ItemToPickup->Destroy();
				return;
			}
		}
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in UObsidianHeroComponent::ServerPickupItemDef_Implementation."));
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

	if(ItemInstance->IsItemEquippable())
	{
		UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
		if(EquipmentComponent == nullptr)
		{
			UE_LOG(LogInventory, Error, TEXT("EquipmentComponent is null in UObsidianHeroComponent::ServerPickupItemDef_Implementation."));
			return;
		}
			
		if(EquipmentComponent->AutomaticallyEquipItem(ItemInstance))
		{
			ItemToPickup->Destroy();
			return;
		}
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in UObsidianHeroComponent::ServerPickupItemInstance_Implementation."));
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

void UObsidianHeroComponent::ServerHandleDroppingItem_Implementation(const FVector& HitLocation)
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}

	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in UObsidianHeroComponent::ServerHandleDroppingItem_Implementation."));
		return;
	}
	
	const FTransform ItemSpawnTransform = FTransform(FRotator::ZeroRotator, HitLocation, FVector(1.0f, 1.0f, 1.0f));
	AObsidianDroppableItem* Item = World->SpawnActorDeferred<AObsidianDroppableItem>(DroppableItemClass, ItemSpawnTransform);
	Item->InitializeItem(DraggedItem);
	Item->FinishSpawning(ItemSpawnTransform);
	DraggedItem.Clear();

	StopDraggingItem(Controller);
}

void UObsidianHeroComponent::ServerGrabDroppableItemToCursor_Implementation(AObsidianDroppableItem* ItemToPickup)
{
	if(ItemToPickup == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("ItemToPickup is null in UObsidianHeroComponent::ServerGrabDroppableItemToCursor_Implementation."));
		return;
	}

	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in UObsidianHeroComponent::ServerGrabDroppableItemToCursor_Implementation."));
		return;
	}
	
	const FPickupTemplate Template = ItemToPickup->GetPickupTemplateFromPickupContent();
	if(Template.IsValid()) // We are grabbing Item Template
	{
		DraggedItem = FDraggedItem(Template.ItemDef, Template.StackCount);
		ItemToPickup->UpdateDroppedItemStacks(0);
		
		StartDraggingItem(Controller);
		return;
	}

	const FPickupInstance Instance = ItemToPickup->GetPickupInstanceFromPickupContent();
	if(Instance.IsValid()) // We are grabbing Item Instance
	{
		DraggedItem = FDraggedItem(Instance.Item);
		ItemToPickup->UpdateDroppedItemStacks(0);
		
		StartDraggingItem(Controller);
		return;
	}

	checkf(false, TEXT("Provided ItemToPickup has no Instance nor Taplate to pick up, this is bad and should not happen."))
}

void UObsidianHeroComponent::ServerGrabInventoryItemToCursor_Implementation(const FVector2D& SlotPosition)
{
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in UObsidianHeroComponent::ServerGrabInventoryItemToCursor_Implementation."));
		return;
	}
	
	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in UObsidianHeroComponent::ServerGrabInventoryItemToCursor_Implementation."));
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = InventoryComponent->GetItemInstanceAtLocation(SlotPosition);
	if(InstanceToGrab == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InstanceToGrab is null in UObsidianHeroComponent::ServerGrabInventoryItemToCursor_Implementation."));
		return;
	}

	InventoryComponent->RemoveItemInstance(InstanceToGrab);
	
	DraggedItem = FDraggedItem(InstanceToGrab);

	StartDraggingItem(Controller);
}

void UObsidianHeroComponent::ServerEquipItemAtSlot_Implementation(const FGameplayTag& SlotTag)
{
	if(DraggedItem.IsEmpty())
	{
		UE_LOG(LogEquipment, Error, TEXT("Tried to add Inventory Item to the Inventory at specific slot but the Dragged Item is Empty in UObsidianHeroComponent::ServerEquipItemAtSlot_Implementation."));
		return;
	}

	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("OwningActor is null in UObsidianHeroComponent::ServerEquipItemAtSlot_Implementation."));
		return;
	}

	UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
	if(EquipmentComponent == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("EquipmentComponent is null in UObsidianHeroComponent::ServerEquipItemAtSlot_Implementation."));
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
		if(EquipmentComponent->EquipItemToSpecificSlot(ItemDef, SlotTag))
		{
			DraggedItem.Clear();
			StopDraggingItem(Controller);
		}
	}
}

void UObsidianHeroComponent::ServerGrabEquippedItemToCursor_Implementation(const FGameplayTag& SlotTag)
{
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("OwningActor is null in UObsidianHeroComponent::ServerGrabEquippedItemToCursor_Implementation."));
		return;
	}
	
	UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
	if(EquipmentComponent == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("InventoryComponent is null in UObsidianHeroComponent::ServerGrabEquippedItemToCursor_Implementation."));
		return;
	}

	UObsidianInventoryItemInstance* InstanceToGrab = EquipmentComponent->GetEquippedInstanceAtSlot(SlotTag);
	if(InstanceToGrab == nullptr)
	{
		UE_LOG(LogEquipment, Error, TEXT("InstanceToGrab is null in UObsidianHeroComponent::ServerGrabEquippedItemToCursor_Implementation."));
		return;
	}

	EquipmentComponent->UnequipItem(InstanceToGrab);
	
	DraggedItem = FDraggedItem(InstanceToGrab);

	StartDraggingItem(Controller);
}

void UObsidianHeroComponent::ServerAddItemToInventoryAtSlot_Implementation(const FVector2D& SlotPosition, const bool bShiftDown)
{
	if(DraggedItem.IsEmpty())
	{
		UE_LOG(LogInventory, Error, TEXT("Tried to add Inventory Item to the Inventory at specific slot but the Dragged Item is Empty in UObsidianHeroComponent::ServerAddItemToInventoryAtSpecificSlot_Implementation."));
		return;
	}

	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in UObsidianHeroComponent::ServerAddItemToInventoryAtSpecificSlot_Implementation."));
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in UObsidianHeroComponent::ServerAddItemToInventoryAtSpecificSlot_Implementation."));
		return;
	}
	
	const int32 StacksToAddOverride = bShiftDown ? 1 : -1;
	
	if(UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		if(InventoryComponent->AddItemInstanceToSpecificSlot(Instance, SlotPosition, StacksToAddOverride))
		{
			DraggedItem.Clear();
			StopDraggingItem(Controller);
			return;
		}
		const int32 CurrentStacks = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		UpdateStacksOnDraggedItemWidget(CurrentStacks);
		DraggedItem.Stacks = CurrentStacks;
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		const int32 ItemStackCount = DraggedItem.Stacks;
		int32 StackLeft = ItemStackCount;
		InventoryComponent->AddItemDefinitionToSpecifiedSlot(ItemDef, SlotPosition, StackLeft, ItemStackCount, StacksToAddOverride);
		if(StackLeft == 0)
		{
			DraggedItem.Clear();
			StopDraggingItem(Controller);
			return;
		}
		UpdateStacksOnDraggedItemWidget(StackLeft);
		DraggedItem.Stacks = StackLeft;
	}
}

void UObsidianHeroComponent::ServerAddStacksFromDraggedItemToItemAtSlot_Implementation(const FVector2D& SlotPosition, const int32 StacksToAddOverride)
{
	if(DraggedItem.IsEmpty())
	{
		UE_LOG(LogInventory, Error, TEXT("Tried to add Inventory Item to the Inventory at specific slot but the Dragged Item is Empty in UObsidianHeroComponent::ServerAddStacksFromDraggedItemToItemAtSlot_Implementation."));
		return;
	}

	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in UObsidianHeroComponent::ServerAddStacksFromDraggedItemToItemAtSlot_Implementation."));
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in UObsidianHeroComponent::ServerAddStacksFromDraggedItemToItemAtSlot_Implementation."));
		return;
	}
	
	UObsidianInventoryItemInstance* Instance = DraggedItem.Instance;
	if(Instance && Instance->IsStackable())
	{
		FObsidianAddingStacksResult AddingStacksResult;
		if(InventoryComponent->TryAddingStacksToSpecificSlotWithInstance(Instance, SlotPosition, /** OUT */ AddingStacksResult, StacksToAddOverride))
		{
			if(AddingStacksResult.bAddedWholeItemAsStacks)
			{
				DraggedItem.Clear();
				StopDraggingItem(Controller);
				return;
			}
			const int32 CurrentStacks = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
			UpdateStacksOnDraggedItemWidget(CurrentStacks);
			DraggedItem.Stacks = CurrentStacks;
		}
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		const UObsidianInventoryItemDefinition* DefaultObject = ItemDef.GetDefaultObject();
		if(DefaultObject && DefaultObject->IsStackable())
		{
			const int32 ItemStackCount = DraggedItem.Stacks;
			FObsidianAddingStacksResult AddingStacksResult;
			if(InventoryComponent->TryAddingStacksToSpecificSlotWithItemDef(ItemDef, ItemStackCount, SlotPosition, /** OUT */ AddingStacksResult, StacksToAddOverride))
			{
				if(AddingStacksResult.bAddedWholeItemAsStacks)
				{
					DraggedItem.Clear();
					StopDraggingItem(Controller);
					return;
				}
				UpdateStacksOnDraggedItemWidget(AddingStacksResult.StacksLeft);
				DraggedItem.Stacks = AddingStacksResult.StacksLeft;
			}
		}
	}
}

void UObsidianHeroComponent::UseItem(const FVector2D& OnSlotPosition, const bool bLeftShiftDown)
{
	ServerUseItem(UsingItemInstance, OnSlotPosition);

	if(bLeftShiftDown == false)
	{
		SetUsingItem(false);
	}
}

void UObsidianHeroComponent::ServerUseItem_Implementation(UObsidianInventoryItemInstance* UsingInstance, const FVector2D& OnSlotPosition)
{
	if(UsingInstance == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("UsingInstance is null in UObsidianHeroComponent::ServerUseItem_Implementation."));
		return;
	}
	
	const AController* Controller = GetController<AController>();
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in UObsidianHeroComponent::ServerUseItem_Implementation."));
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in UObsidianHeroComponent::ServerUseItem_Implementation."));
		return;
	}

	UObsidianInventoryItemInstance* UsingOntoInstance = InventoryComponent->GetItemInstanceAtLocation(OnSlotPosition);
	InventoryComponent->UseItem(UsingInstance, UsingOntoInstance);
}

void UObsidianHeroComponent::UpdateStacksOnDraggedItemWidget(const int32 InStacks)
{
	if(DraggedItemWidget)
	{
		DraggedItemWidget->UpdateStackCount(InStacks);
	}
}

bool UObsidianHeroComponent::DraggedItemWasReplaced(const FDraggedItem& OldDraggedItem) const
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

void UObsidianHeroComponent::StartDraggingItem(const AController* Controller)
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
	
	checkf(DraggedItemWidgetClass, TEXT("DraggedItemWidgetClass is invalid in UObsidianHeroComponent::DragItem please fill it on ObsidianDroppableItem Instance."));
	UObsidianDraggedItem* Item = CreateWidget<UObsidianDraggedItem>(World, DraggedItemWidgetClass);

	bool bInitialized = false;
	if(UObsidianInventoryItemInstance* Instance = DraggedItem.Instance)
	{
		Item->InitializeItemWidgetWithItemInstance(Instance);
		bInitialized = true;
	}
	else if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		Item->InitializeItemWidgetWithItemDef(ItemDef, DraggedItem.Stacks);
		bInitialized = true;
	}
	checkf(bInitialized, TEXT("Item was not initialized with neither Instance nor ItemDef, this is bad and should not happen."));
	
	Item->AddToViewport();
	
	DraggedItemWidget = Item;
	bDraggingItem = true;

	TWeakObjectPtr<UObsidianHeroComponent> WeakThis(this);
	World->GetTimerManager().SetTimerForNextTick([WeakThis]()
	{
		if(WeakThis.IsValid())
		{
			WeakThis->bItemAvailableForDrop = true;
		}
	});
}

void UObsidianHeroComponent::StopDraggingItem(const AController* Controller)
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
}

bool UObsidianHeroComponent::CanDropItem() const
{
	return !bCursorOverUI && IsDraggingAnItem() && bItemAvailableForDrop;
}

bool UObsidianHeroComponent::CanMoveMouse() const
{
	return !CanDropItem() && !bJustDroppedItem;
}



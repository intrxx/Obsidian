// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Items/ObsidianDroppableItem.h"

// ~ Core
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// ~ Project
#include "CharacterComponents/ObsidianPlayerInputManager.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Components/SplineComponent.h"
#include "Components/TimelineComponent.h"
#include "Core/FunctionLibraries/ObsidianUIFunctionLibrary.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/Inventory/Items/ObsidianItemWorldName.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "UI/ObsidianHUD.h"
#include "UI/Inventory/Items/ObsidianItemDescriptionBase.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"

AObsidianDroppableItem::AObsidianDroppableItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootSceneComponent->SetRelativeRotation(FRotator::ZeroRotator);
	SetRootComponent(RootSceneComponent);
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMeshComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	StaticMeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	StaticMeshComp->SetCollisionResponseToChannel(Obsidian_TraceChannel_PlayerCursorTrace, ECR_Block);
	StaticMeshComp->SetCustomDepthStencilValue(ObsidianHighlight::White);
	StaticMeshComp->SetRenderCustomDepth(false);
	StaticMeshComp->SetupAttachment(RootSceneComponent);
	
	WorldItemNameWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WorldItemNameWidgetComp"));
	WorldItemNameWidgetComp->SetupAttachment(GetRootComponent());
	WorldItemNameWidgetComp->SetRelativeLocation(FVector(0.0f, 50.0f, 0.0f));
	WorldItemNameWidgetComp->SetDrawAtDesiredSize(true);
	WorldItemNameWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	WorldItemNameWidgetComp->SetupAttachment(StaticMeshComp);
	WorldItemNameWidgetComp->SetVisibility(false);

	static ConstructorHelpers::FClassFinder<UUserWidget> ItemLabelClassFinder(TEXT("/Game/Obsidian/UI/GameplayUserInterface/Inventory/Items/WBP_ItemWorldName.WBP_ItemWorldName_C"));
	if (ItemLabelClassFinder.Succeeded())
	{
		WorldItemNameWidgetComp->SetWidgetClass(ItemLabelClassFinder.Class);
	}
#if !UE_BUILD_SHIPPING
	else
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("ItemLabelClassFinder cannot find the default ItemLabelClass for WorldItemNameWidgetComp in AObsidianDroppableItem's constructor"
													   " previously located in /Game/Obsidian/UI/GameplayUserInterface/Inventory/Items/WBP_ItemWorldName.WBP_ItemWorldName_C")), ELogVerbosity::Error);
	}
#endif
	
	static ConstructorHelpers::FObjectFinder<UCurveFloat> DropCurveClassFinder(TEXT("/Game/Obsidian/Items/ItemDrop/Curves/ItemDrop_FloatCurve.ItemDrop_FloatCurve"));
	if (DropCurveClassFinder.Succeeded())
	{
		DropLocationCurve = DropCurveClassFinder.Object;
	}
#if !UE_BUILD_SHIPPING
	else
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("DropCurveClassFinder cannot find DropLocationCurve in AObsidianDroppableItem's constructor"
													   " previously located in /Game/Obsidian/Items/ItemDrop/Curves/ItemDrop_FloatCurve.ItemDrop_FloatCurve")), ELogVerbosity::Error);
	}
#endif
}

void AObsidianDroppableItem::BeginPlay()
{
	Super::BeginPlay();

	// In some edge conditions the Replication of actual item set on this ObsidianDroppableItem can not happen before BeginPlay
	// In This special conditions we will try to set the name again in OnRep_PickupContent if it was not yet initialized
	if(bInitializedItemName == false)
	{
		bInitializedItemName = InitializeWorldName();
	}

	InitDropRouteAnimation();
}

void AObsidianDroppableItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, DroppedItemStacks);
}

void AObsidianDroppableItem::InitializeItem(const FDraggedItem& DraggedItem)
{
	if(HasAuthority() == false)
	{
		return;
	}

	if(!ensureMsgf(DraggedItem.IsEmpty() == false, TEXT("Tried to Initialize Item in AObsidianDroppableItem::InitializeItem but the DraggedItem is null.")))
	{
		return;
	}
	
	if(UObsidianInventoryItemInstance* ItemInstance = DraggedItem.Instance)
	{
		AddItemInstance(ItemInstance);
		return;
	}

	if(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = DraggedItem.ItemDef)
	{
		AddItemDefinition(ItemDef, DraggedItem.GeneratedData);
		return;
	}

	checkf(false, TEXT("Failed to Initialize Item with neither Item Def nor Instance, something is wrong."));
}

void AObsidianDroppableItem::InitializeItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FObsidianItemGeneratedData& InGeneratedData)
{
	if(HasAuthority() == false)
	{
		return;
	}

	if(!ensureMsgf(ItemDef, TEXT("Tried to Initialize Item in AObsidianDroppableItem::InitializeItem but the DraggedItem is null.")))
	{
		return;
	}
	
	if(ItemDef)
	{
		AddItemDefinition(ItemDef, InGeneratedData);
		return;
	}
}

bool AObsidianDroppableItem::InitializeWorldName()
{
	UWorld* World = GetWorld();
	if(World == nullptr || bInitializedItemName)
	{
		return false;
	}

	if (WorldItemNameWidgetComp == nullptr)
	{
		return false;
	}

	bool bSuccess = false;
	ItemWorldName = Cast<UObsidianItemWorldName>(WorldItemNameWidgetComp->GetUserWidgetObject());
	if (ItemWorldName)
	{
		ItemWorldName->OnItemWorldNameMouseHoverDelegate.AddUObject(this, &ThisClass::OnItemMouseHover);
		ItemWorldName->OnItemWorldNameMouseButtonDownDelegate.AddUObject(this, &ThisClass::OnItemMouseButtonDown);
		bSuccess = InitItemWorldName();
	}
	
	return bSuccess;
}

void AObsidianDroppableItem::OnRep_PickupContent()
{
	if(CarriesItemDef())
	{
		SetupItemAppearanceFromDefinition();
	}
	else if(CarriesItemInstance())
	{
		SetupItemAppearanceFromInstance();
	}

	if(bInitializedItemName == false)
	{
		bInitializedItemName = InitializeWorldName();
	}
}

void AObsidianDroppableItem::AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd)
{
	Super::AddItemInstance(InstanceToAdd);

	SetupItemAppearanceFromInstance();
}

void AObsidianDroppableItem::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FObsidianItemGeneratedData& InGeneratedData)
{
	Super::AddItemDefinition(ItemDef, InGeneratedData);

	SetupItemAppearanceFromDefinition();
}

AActor* AObsidianDroppableItem::GetHighlightAvatarActor()
{
	return this;
}

void AObsidianDroppableItem::StartHighlight()
{
	if(IsValid(ItemWorldName))
	{
		ItemWorldName->HandleWorldNameHighlightBegin();
	}
	OnItemMouseHover(true);
}

void AObsidianDroppableItem::StopHighlight()
{
	if(IsValid(ItemWorldName))
	{
		ItemWorldName->HandleWorldNameHighlightEnd();
	}
	OnItemMouseHover(false);
}

AActor* AObsidianDroppableItem::GetInteractionActor()
{
	return this;
}

bool AObsidianDroppableItem::CanInteract()
{
	return true;
}

float AObsidianDroppableItem::GetInteractionRadius()
{
	return 0.0f; // Get the Interaction Radius from Hero Comp
}

void AObsidianDroppableItem::Interact(AObsidianPlayerController* InteractingPlayerController)
{
	if(InteractingPlayerController == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Cannot interact with Item Actor, ObsidianPC is invalid in AObsidianDroppableItem::Interact."))
		return;
	}

	if(CarriesItemDef())
	{
		PickupItemDef(false /**TODO This is no consistent, leave it for now tho : < */, InteractingPlayerController);
	}
	else if(CarriesItemInstance())
	{
		PickupItemInstance(false /**TODO This is no consistent, leave it for now tho : < */, InteractingPlayerController);
	}
}

void AObsidianDroppableItem::UpdateDroppedItemStacks(const int32 NewDroppedItemStacks)
{
	if(NewDroppedItemStacks > 0 && DroppedItemStacks != NewDroppedItemStacks)
	{
		DroppedItemStacks = NewDroppedItemStacks;
		
		if(CarriesItemDef())
		{
			OverrideTemplateStacks(DroppedItemStacks);
		}
		
		UpdateStacksOnActiveItemDescription(DroppedItemStacks);
		return;
	}
	
#if !UE_BUILD_SHIPPING
	if(NewDroppedItemStacks < 0)
	{
		UE_LOG(LogInventory, Error, TEXT("AObsidianDroppableItem::UpdateDroppedItemStacks shouldn't take a negative number of stacks to update."));
	}
#endif
	
	DestroyDroppedItem();
}

void AObsidianDroppableItem::DestroyDroppedItem()
{
	Destroy();
}

void AObsidianDroppableItem::Destroyed()
{
	if(const AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Got PC [%s]"), *GetNameSafe(ObsidianPC));
		if(ObsidianPC->IsLocalPlayerController())
		{
			StopHighlight();
			DestroyItemDescription();
		}
	}
	
	Super::Destroyed();
}

void AObsidianDroppableItem::SetupItemAppearanceFromInstance() const
{
	const UObsidianInventoryItemInstance* ItemInstance = GetPickupInstanceFromPickupContent().Item;
	if(ItemInstance == nullptr)
	{
		return;
	}

	UStaticMesh* DroppedMesh = ItemInstance->GetItemDroppedMesh();
	if(DroppedMesh == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Item [%s] failed to set the Dropped Mesh in: AObsidianDroppableItem::SetupItemAppearanceFromDefinition"),
			*ItemInstance->GetItemDebugName());
		return;
	}

	StaticMeshComp->SetStaticMesh(DroppedMesh);
}

void AObsidianDroppableItem::SetupItemAppearanceFromDefinition() const
{
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = GetPickupTemplateFromPickupContent().ItemDef;
	if(!ensureMsgf(ItemDef, TEXT("Item Def is invalid in AObsidianDroppableItem::SetupItemAppearanceFromDefinition, make sure the logic is correct.")))
	{
		return;
	}
	
	const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef);
	if(!ensureMsgf(ItemDefault, TEXT("Failed to extract Default from Item Def in AObsidianDroppableItem::SetupItemAppearanceFromDefinition.")))
	{
		return;
	}
	
	if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
	{
		if(UStaticMesh* DroppedMesh = AppearanceFrag->GetItemDroppedMesh())
		{
			StaticMeshComp->SetStaticMesh(DroppedMesh);
			return;
		}
	}
	
	UE_LOG(LogInventory, Error, TEXT("Item [%s] failed to set the Dropped Mesh in: AObsidianDroppableItem::SetupItemAppearanceFromDefinition."),
		*ItemDefault->GetDebugName());
}

bool AObsidianDroppableItem::InitItemWorldName() const
{
	if(!IsValid(ItemWorldName))
	{
		UE_LOG(LogInventory, Error, TEXT("Item World Name is invalid in AObsidianDroppableItem::InitItemWorldName."));
		return false;
	}
	bool bSuccess = false;

	if(const TSubclassOf<UObsidianInventoryItemDefinition> PickupItemDef = GetPickupTemplateFromPickupContent().ItemDef)
	{
		if(const UObsidianInventoryItemDefinition* DefaultItem = PickupItemDef.GetDefaultObject())
		{
			if(const UOInventoryItemFragment_Appearance* Appearance = Cast<UOInventoryItemFragment_Appearance>(DefaultItem->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
			{
				const FText ItemDisplayName = Appearance->GetItemDisplayName();
				ItemWorldName->SetItemName(ItemDisplayName);
				bSuccess = true;
			}
		}
	}
	else if(const UObsidianInventoryItemInstance* ItemInstance = GetPickupInstanceFromPickupContent().Item)
    {
         const FText ItemDisplayName = ItemInstance->GetItemDisplayName();
         ItemWorldName->SetItemName(ItemDisplayName);
		bSuccess = true;
    }
	
	if(!bSuccess)
	{
		UE_LOG(LogInventory, Error, TEXT("Failed to setup Item World Name in AObsidianDroppableItem::InitItemWorldName."));
	}
	return bSuccess;
}

void AObsidianDroppableItem::InitDropRouteAnimation()
{
	const AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if(ObsidianPC == nullptr || ObsidianPC->IsLocalPlayerController() == false)
	{
		return;
	}
	
	// Maybe create it with NewObject like timeline?
	ItemDropSplineComp = Cast<USplineComponent>(
		AddComponentByClass(USplineComponent::StaticClass(), true, FTransform::Identity, false)
		);

	if(ItemDropSplineComp)
	{
		const FVector CurrentActorLocation = GetActorLocation();
		const FVector InitialLocation = FVector(CurrentActorLocation.X, CurrentActorLocation.Y, CurrentActorLocation.Z + 75.0f);
		const FVector MidPointLocation = FMath::Lerp(InitialLocation, CurrentActorLocation, 0.50f) + FVector(0.0f, 0.0f, 100.0f);
		const TArray<FVector> ItemDropRoute = {InitialLocation, MidPointLocation, CurrentActorLocation};
		
		ItemDropSplineComp->SetSplinePoints(ItemDropRoute, ESplineCoordinateSpace::World, true);
	}

	FinalItemRotation = GetActorRotation();
	
	if(FinalItemRotation == FRotator::ZeroRotator) // If the item was placed in the level it will have Zero Rotation, randomize the Yaw. //TODO(intrxx) This check will fail if someone set the rotation which I don't expect to do but ¯\_(ツ)_/¯ 
	{
		FinalItemRotation.Yaw += FMath::FRandRange(0.0f, 180.0f);
	}
	
	InitialItemRotation = FRotator(FinalItemRotation.Pitch, FinalItemRotation.Yaw + FMath::FRandRange(-30.0f, 30.0f), FinalItemRotation.Roll);

	UTimelineComponent* ItemDropTimelineComp = NewObject<UTimelineComponent>(this);
	ItemDropTimelineComp->SetLooping(false);
	ItemDropTimelineComp->RegisterComponent();

	if(ItemDropTimelineComp && DropLocationCurve)
	{
		FOnTimelineFloat ProgressUpdate;
		ProgressUpdate.BindDynamic(this, &ThisClass::UpdateItemDropAnimation);
		ItemDropTimelineComp->AddInterpFloat(DropLocationCurve, ProgressUpdate);

		FOnTimelineEventStatic OnFinished;
		OnFinished.BindLambda([this, ItemDropTimelineComp]()
			{
				if(ItemDropTimelineComp)
				{
					ItemDropTimelineComp->DestroyComponent();
				}
				if(ItemDropSplineComp)
				{
					ItemDropSplineComp->DestroyComponent();
				}
				if(WorldItemNameWidgetComp)
				{
					WorldItemNameWidgetComp->SetVisibility(true);
				}
			});

		ItemDropTimelineComp->SetTimelineFinishedFunc(OnFinished);
		ItemDropTimelineComp->PlayFromStart();
	}
}

void AObsidianDroppableItem::UpdateItemDropAnimation(float UpdateAlpha)
{
	if(ItemDropSplineComp == nullptr)
	{
		return;
	}
	
	const FVector NewLocation = ItemDropSplineComp->GetLocationAtTime(UpdateAlpha, ESplineCoordinateSpace::World, false);

	const float NewYaw = FMath::Lerp(InitialItemRotation.Yaw, FinalItemRotation.Yaw, UpdateAlpha);
	const float NewRoll = FMath::Lerp(InitialItemRotation.Roll, FinalItemRotation.Roll + 270.0f, UpdateAlpha);
	const FRotator NewRotation = FRotator(FinalItemRotation.Pitch, NewYaw, NewRoll);

	//NOTE(intrxx) SetActorLocationAndRotation(NewLocation, NewRotation); We can't just use the Actor's Location and Rotation as this will replicate to Clients
	// when set on Listen Server as we want to play the anim for Local PLayer Controller which in one case happens to be Listen Server.
	// In order to bypass this I needed to create SceneComponent and make it RootComponent so the Mesh wouldn't affect
	// actual Actor Transform (as it was previously RootComponent) and then just set the location on the Mesh, additionally this might be faster
	// due to using this NoPhysics version - I don't use physics on dropped items anyway.
	// https://github.com/intrxx/Obsidian/commit/d000d45c49bfeae0597347f75d9e4dc8c8e61642
	StaticMeshComp->SetWorldLocationAndRotationNoPhysics(NewLocation, NewRotation);
}

void AObsidianDroppableItem::OnItemMouseHover(const bool bMouseEnter)
{
	if(StaticMeshComp)
	{
		StaticMeshComp->SetRenderCustomDepth(bMouseEnter);
	}

	if(bMouseEnter)
	{
		CreateItemDescription();
	}
	else
	{
		DestroyItemDescription();
	}
}

void AObsidianDroppableItem::CreateItemDescription()
{
	CachedInventoryWidgetController = CachedInventoryWidgetController == nullptr ? UObsidianUIFunctionLibrary::GetInventoryItemsWidgetController(this) : CachedInventoryWidgetController;
	if(CachedInventoryWidgetController == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Unable to get InventoryController in AObsidianDroppableItem::CreateItemDescription."));
		return;	
	}
	
	if(CarriesItemDef())
	{
		const FObsidianPickupTemplate PickupTemplate = GetPickupTemplateFromPickupContent();
		CachedInventoryWidgetController->CreateItemDescriptionForDroppedItem(PickupTemplate.ItemDef, PickupTemplate.ItemGeneratedData);
	}
	else if(CarriesItemInstance())
	{
		CachedInventoryWidgetController->CreateItemDescriptionForDroppedItem(GetPickupInstanceFromPickupContent().Item);
	}
}

void AObsidianDroppableItem::DestroyItemDescription()
{
	const bool server = HasAuthority();
	UE_LOG(LogTemp, Warning, TEXT("%s"), server ? TEXT("Server") : TEXT("Client"));
	
	CachedInventoryWidgetController = CachedInventoryWidgetController == nullptr ? UObsidianUIFunctionLibrary::GetInventoryItemsWidgetController(this) : CachedInventoryWidgetController;
	if(CachedInventoryWidgetController)
	{
		CachedInventoryWidgetController->RemoveCurrentItemDescription();
		return;	
	}
	UE_LOG(LogInventory, Error, TEXT("Unable to get InventoryController in AObsidianDroppableItem::DestroyItemDescription."));
}

void AObsidianDroppableItem::UpdateStacksOnActiveItemDescription(const UObsidianInventoryItemInstance* ItemInstance)
{
	CachedInventoryWidgetController = CachedInventoryWidgetController == nullptr ? UObsidianUIFunctionLibrary::GetInventoryItemsWidgetController(this) : CachedInventoryWidgetController;
	if(CachedInventoryWidgetController == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Unable to get InventoryController in AObsidianDroppableItem::UpdateStacksOnActiveItemDescription."));
		return;	
	}
	
	if(UObsidianItemDescriptionBase* ActiveItemDescription = CachedInventoryWidgetController->GetActiveItemDescription())
	{
		int32 CurrentStacks = 0;
		if(!ensureMsgf(ItemInstance, TEXT("Item Instance is invalid in AObsidianDroppableItem::UpdateStacksOnActiveItemDescription, stacks were set to 0.")))
		{
			CurrentStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		}
		ActiveItemDescription->UpdateCurrentStackCount(CurrentStacks);
	}
}

void AObsidianDroppableItem::UpdateStacksOnActiveItemDescription(const int32 StacksToSet)
{
	CachedInventoryWidgetController = CachedInventoryWidgetController == nullptr ? UObsidianUIFunctionLibrary::GetInventoryItemsWidgetController(this) : CachedInventoryWidgetController;
	if(CachedInventoryWidgetController == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Unable to get InventoryController in AObsidianDroppableItem::CreateItemDescription."));
		return;	
	}
	
	if(UObsidianItemDescriptionBase* ActiveItemDescription = CachedInventoryWidgetController->GetActiveItemDescription())
	{
		ActiveItemDescription->UpdateCurrentStackCount(StacksToSet);
	}
}

void AObsidianDroppableItem::OnRep_DroppedItemStacks()
{
	if(CarriesItemDef())
	{
		OverrideTemplateStacks(DroppedItemStacks);
	}
	UpdateStacksOnActiveItemDescription(DroppedItemStacks);
}

void AObsidianDroppableItem::OnItemMouseButtonDown(const int32 PlayerIndex, const FObsidianItemInteractionFlags& InteractionFlags)
{
	const UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}
	
	const AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(World, PlayerIndex));
	if(!ensureMsgf(ObsidianPC, TEXT("Failed to acquire valid ObsidianPlayerController in AObsidianDroppableItem::OnItemMouseButtonDown.")))
	{
		return;
	}
	
	if(CarriesItemDef())
	{
		PickupItemDef(InteractionFlags.bAutomaticallyAddToWindow, ObsidianPC);
	}
	else if(CarriesItemInstance())
	{
		PickupItemInstance(InteractionFlags.bAutomaticallyAddToWindow, ObsidianPC);
	}
}

bool AObsidianDroppableItem::PickupItemInstance(const bool bLeftControlDown, const AObsidianPlayerController* PickingPlayerController)
{
	AObsidianHUD* ObsidianHUD = PickingPlayerController->GetObsidianHUD();
	if(ObsidianHUD == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Cannot Pickup Item Def, ObsidianHUD is invalid in AObsidianDroppableItem::PickupItemInstance."))
		return false;
	}
	
	const AObsidianHero* Hero = Cast<AObsidianHero>(PickingPlayerController->GetCharacter());
	checkf(Hero, TEXT("Hero acquired from ObsidianPC is invalid in AObsidianDroppableItem::PickupItemDef."));
	
	UObsidianPlayerInputManager* PlayerInputManager = Hero->GetPlayerInputManager();
	checkf(PlayerInputManager, TEXT("PlayerInputManager acquired from OwningActor is invalid in AObsidianDroppableItem::PickupItemDef."));
	
	const bool bIsDraggingAnItem = PlayerInputManager->IsDraggingAnItem();
	if(ObsidianHUD->IsInventoryOpened() && !bLeftControlDown) // If the inventory is opened, and we don't press the left control button spawn the item (with its whole stacks) on cursor.
	{
		bool bDroppedItem = false;
		if(bIsDraggingAnItem)
		{
			bDroppedItem = PlayerInputManager->DropItem();
		}

		if((!bIsDraggingAnItem) || (bIsDraggingAnItem && bDroppedItem))
		{
			PlayerInputManager->ServerGrabDroppableItemToCursor(this);
			return true; // Added whole Item
		}
		return false; // Added some Item stacks
	}
	PlayerInputManager->ServerPickupItem(this);
	
	return false; // Added whole Item
}

bool AObsidianDroppableItem::PickupItemDef(const bool bLeftControlDown, const AObsidianPlayerController* PickingPlayerController)
{
	AObsidianHUD* ObsidianHUD = PickingPlayerController->GetObsidianHUD();
	if(ObsidianHUD == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Cannot Pickup Item Def, ObsidianHUD is invalid in AObsidianDroppableItem::PickupItemDef."))
		return false;
	}
	
	const AObsidianHero* Hero = Cast<AObsidianHero>(PickingPlayerController->GetCharacter());
	checkf(Hero, TEXT("Hero acquired from ObsidianPC is invalid in AObsidianDroppableItem::PickupItemDef."));
	
	UObsidianPlayerInputManager* PlayerInputManager = Hero->GetPlayerInputManager();
	checkf(PlayerInputManager, TEXT("PlayerInputManager acquired from OwningActor is invalid in AObsidianDroppableItem::PickupItemDef."));
	
	const bool bIsDraggingAnItem = PlayerInputManager->IsDraggingAnItem();
	if(ObsidianHUD->IsInventoryOpened() && !bLeftControlDown) // If the inventory is opened, and we don't press left control button, spawn the item on cursor
	{
		bool bDroppedItem = false;
		if(bIsDraggingAnItem)
		{
			bDroppedItem = PlayerInputManager->DropItem();
		}

		if((!bIsDraggingAnItem) || (bIsDraggingAnItem && bDroppedItem))
		{
			PlayerInputManager->ServerGrabDroppableItemToCursor(this);
			return true; // Added whole Item
		}
		return false; // Added some Item stacks
	}
	PlayerInputManager->ServerPickupItem(this);
	
	return false; // Added whole Item
}




// Copyright 2024 out of sCope team - Michał Ogiński

#include "Gameplay/InventoryItems/ObsidianDroppableItem.h"

// ~ Core
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// ~ Project
#include "CharacterComponents/ObsidianHeroComponent.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Components/SplineComponent.h"
#include "Components/TimelineComponent.h"
#include "Core/ObsidianUIFunctionLibrary.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/Inventory/ObsidianItemWorldName.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "UI/ObsidianHUD.h"
#include "UI/Inventory/ObsidianItemDescriptionBase.h"
#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"

AObsidianDroppableItem::AObsidianDroppableItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMeshComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	StaticMeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	StaticMeshComp->SetCollisionResponseToChannel(Obsidian_TraceChannel_PlayerCursorTrace, ECR_Block);
	StaticMeshComp->SetCustomDepthStencilValue(ObsidianHighlight::White);
	StaticMeshComp->SetRenderCustomDepth(false);
	//StaticMeshComp->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	SetRootComponent(StaticMeshComp);
	
	WorldItemNameWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WorldItemNameWidgetComp"));
	WorldItemNameWidgetComp->SetupAttachment(GetRootComponent());
	WorldItemNameWidgetComp->SetRelativeLocation(FVector(0.0f, 50.0f, 0.0f));
	WorldItemNameWidgetComp->SetDrawAtDesiredSize(true);
	WorldItemNameWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	WorldItemNameWidgetComp->SetupAttachment(StaticMeshComp);
	
	OnClicked.AddDynamic(this, &ThisClass::HandleActorClicked);
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
		const int32 Stacks = DraggedItem.Stacks;
		AddItemDefinition(ItemDef, Stacks);
		return;
	}

	checkf(false, TEXT("Failed to Initialize Item with neither Item Def nor Instance, something is wrong."));
}

bool AObsidianDroppableItem::InitializeWorldName()
{
	UWorld* World = GetWorld();
	if(World == nullptr || bInitializedItemName)
	{
		return false;
	}
	
	checkf(ItemWorldNameClass, TEXT("ItemWorldNameClass is invalid, please make sure it is set in ObsidianDroppableItem instance."));
	ItemWorldName = CreateWidget<UObsidianItemWorldName>(World, ItemWorldNameClass);
	ItemWorldName->OnItemWorldNameMouseHoverDelegate.AddUObject(this, &ThisClass::OnItemMouseHover);
	ItemWorldName->OnItemWorldNameMouseButtonDownDelegate.AddUObject(this, &ThisClass::OnItemMouseButtonDown);
	const bool bSuccess = InitItemWorldName();
	
	WorldItemNameWidgetComp->SetWidget(ItemWorldName);
	WorldItemNameWidgetComp->InitWidget();

	return bSuccess;
}

void AObsidianDroppableItem::Destroyed()
{
	DestroyItemDescription();
	
	Super::Destroyed();
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

void AObsidianDroppableItem::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 ItemStacks)
{
	Super::AddItemDefinition(ItemDef, ItemStacks);

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

void AObsidianDroppableItem::UpdateDroppedItemStacks(const int32 NewDroppedItemStacks)
{
	if(NewDroppedItemStacks > 0)
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
	Destroy();
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

void AObsidianDroppableItem::HandleActorClicked(AActor* AffectedActor, FKey ButtonPressed)
{
	if(ButtonPressed == EKeys::LeftMouseButton)
	{
		//TODO This is incorrect for multiplayer, I need to use HeroComponent for that to know the correct Player that wants to pick up the item
		const bool bLeftControlDown = FSlateApplication::Get().GetModifierKeys().IsLeftControlDown();
		ensure(false);
		//OnItemMouseButtonDown(bLeftControlDown);
	}
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
	// Maybe create it with NewObject like timeline?
	ItemDropSplineComp = Cast<USplineComponent>(
		AddComponentByClass(USplineComponent::StaticClass(), true, FTransform::Identity, false)
		);

	if(ItemDropSplineComp)
	{
		//ItemDropSplineComp->RegisterComponent();
		//ItemDropSplineComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

		const FVector CurrentActorLocation = GetActorLocation();
		const FVector InitialLocation = FVector(CurrentActorLocation.X, CurrentActorLocation.Y, CurrentActorLocation.Z + 75.0f);
		const FVector MidPointLocation = FMath::Lerp(InitialLocation, CurrentActorLocation, 0.50f) + FVector(0.0f, 0.0f, 100.0f);
		const TArray<FVector> ItemDropRoute =
			{
			{InitialLocation},
			{MidPointLocation},
			{CurrentActorLocation}
			};
		
		ItemDropSplineComp->SetSplinePoints(ItemDropRoute, ESplineCoordinateSpace::World, true);
	}

	InitialRotation = GetActorRotation();
	RandomYawRotation = FMath::FRandRange(-30.0f, 30.0f);

	ItemDropTimelineComp = NewObject<UTimelineComponent>(this);
	ItemDropTimelineComp->SetLooping(false);
	ItemDropTimelineComp->RegisterComponent();

	if (ItemDropTimelineComp && DropRotationCurve && DropLocationCurve)
	{
		FOnTimelineFloat ProgressUpdate;
		ProgressUpdate.BindDynamic(this, &ThisClass::UpdateItemDropAnimation);
		ItemDropTimelineComp->AddInterpFloat(DropLocationCurve, ProgressUpdate);

		FOnTimelineEventStatic OnFinished;
		OnFinished.BindLambda([this]()
			{
				if(ItemDropTimelineComp)
				{
					ItemDropTimelineComp->DestroyComponent();
				}
				if(ItemDropSplineComp)
				{
					ItemDropSplineComp->DestroyComponent();
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

	const float NewYaw = FMath::Lerp(InitialRotation.Yaw, RandomYawRotation, UpdateAlpha);
	const float NewRoll = FMath::Lerp(InitialRotation.Roll, InitialRotation.Roll + 270.0f, UpdateAlpha);
	const FRotator NewRotation = FRotator(InitialRotation.Pitch, NewYaw, NewRoll);

	SetActorLocationAndRotation(NewLocation, NewRotation);
}

void AObsidianDroppableItem::OnItemMouseHover(const bool bMouseEnter)
{
	if(StaticMeshComp)
	{
		StaticMeshComp->SetRenderCustomDepth(bMouseEnter);
	}

	if(bMouseEnter)
	{
		ActiveItemDescription = CreateItemDescription();
	}
	else
	{
		DestroyItemDescription();
	}
}

UObsidianItemDescriptionBase* AObsidianDroppableItem::CreateItemDescription()
{
	DestroyItemDescription();
	
	UObsidianInventoryWidgetController* InventoryController = UObsidianUIFunctionLibrary::GetInventoryWidgetController(this);
	if(InventoryController == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Unable to get InventoryController in AObsidianDroppableItem::CreateItemDescription."));
		return nullptr;	
	}
	
	UObsidianItemDescriptionBase* ItemDescriptionToReturn = nullptr; 
	if(CarriesItemDef())
	{
		const FPickupTemplate PickupTemplate = GetPickupTemplateFromPickupContent();
		ItemDescriptionToReturn = InventoryController->CreateItemDescriptionForDroppedItem(PickupTemplate.ItemDef, PickupTemplate.StackCount);
	}
	else if(CarriesItemInstance())
	{
		ItemDescriptionToReturn = InventoryController->CreateItemDescriptionForDroppedItem(GetPickupInstanceFromPickupContent().Item);
	}
#if !UE_BUILD_SHIPPING
	else
	{
		ensureMsgf(ItemDescriptionToReturn, TEXT("Returned ItemDescription is invalid in AObsidianDroppableItem::CreateItemDescription(), verify if the Dropped Item contains any Pickup Content."));
	}
#endif
	
	return ItemDescriptionToReturn;
}

void AObsidianDroppableItem::UpdateStacksOnActiveItemDescription(const UObsidianInventoryItemInstance* ItemInstance) const
{
	if(ActiveItemDescription)
	{
		int32 CurrentStacks = 0;
		if(!ensureMsgf(ItemInstance, TEXT("Item Instance is invalid in AObsidianDroppableItem::UpdateStacksOnActiveItemDescription, stacks were set to 0.")))
		{
			CurrentStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		}
		ActiveItemDescription->UpdateCurrentStackCount(CurrentStacks);
	}
}

void AObsidianDroppableItem::UpdateStacksOnActiveItemDescription(const int32 StacksToSet) const
{
	if(ActiveItemDescription)
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

void AObsidianDroppableItem::OnItemMouseButtonDown(const int32 PlayerIndex, const bool bLeftControlDown)
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}
	
	AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(World, PlayerIndex));
	if(!ensureMsgf(ObsidianPC, TEXT("Failed to acquire valid ObsidianPlayerController in AObsidianDroppableItem::OnItemMouseButtonDown.")))
	{
		return;
	}

	// Bool needs to stay here for a while as not every case of picking up item is handled through server authoritative way as of now.
	bool bItemPickedUp = false;
	if(CarriesItemDef())
	{
		bItemPickedUp = PickupItemDef(bLeftControlDown, ObsidianPC);
		//PickupItemDef(bLeftControlDown, ObsidianPC);
	}
	else if(CarriesItemInstance())
	{
		bItemPickedUp = PickupItemInstance(bLeftControlDown, ObsidianPC);
		//PickupItemInstance(bLeftControlDown, ObsidianPC);
	}
	
	if(bItemPickedUp)
	{
		Destroy();
	}
}

bool AObsidianDroppableItem::PickupItemInstance(const bool bLeftControlDown, AObsidianPlayerController* PickingPlayerController)
{
	AObsidianHUD* ObsidianHUD = PickingPlayerController->GetObsidianHUD();
	if(ObsidianHUD == nullptr)
	{
		return false;
	}
	
	const AObsidianHero* Hero = Cast<AObsidianHero>(PickingPlayerController->GetCharacter());
	checkf(Hero, TEXT("Hero acquired from ObsidianPC is invalid in AObsidianDroppableItem::PickupItemDef."));
	
	UObsidianHeroComponent* HeroComp = Hero->GetHeroComponent();
	checkf(HeroComp, TEXT("HeroComp acquired from OwningActor is invalid in AObsidianDroppableItem::PickupItemDef."));
	
	const bool bIsDraggingAnItem = HeroComp->IsDraggingAnItem();
	if(ObsidianHUD->IsInventoryOpened() && !bLeftControlDown) // If the inventory is opened, and we don't press the left control button spawn the item (with its whole stacks) on cursor.
	{
		bool bDroppedItem = false;
		if(bIsDraggingAnItem)
		{
			bDroppedItem = HeroComp->DropItem();
		}

		if((!bIsDraggingAnItem) || (bIsDraggingAnItem && bDroppedItem))
		{
			HeroComp->ServerGrabDroppableItemToCursor(this);
			return true; // Added whole Item
		}
		return false; // Added some Item stacks
	}
	HeroComp->ServerPickupItemInstance(this);
	
	return false; // Added whole Item
}

bool AObsidianDroppableItem::PickupItemDef(const bool bLeftControlDown, AObsidianPlayerController* PickingPlayerController)
{
	AObsidianHUD* ObsidianHUD = PickingPlayerController->GetObsidianHUD();
	if(ObsidianHUD == nullptr)
	{
		return false;
	}
	
	const AObsidianHero* Hero = Cast<AObsidianHero>(PickingPlayerController->GetCharacter());
	checkf(Hero, TEXT("Hero acquired from ObsidianPC is invalid in AObsidianDroppableItem::PickupItemDef."));
	
	UObsidianHeroComponent* HeroComp = Hero->GetHeroComponent();
	checkf(HeroComp, TEXT("HeroComp acquired from OwningActor is invalid in AObsidianDroppableItem::PickupItemDef."));
	
	const bool bIsDraggingAnItem = HeroComp->IsDraggingAnItem();
	if(ObsidianHUD->IsInventoryOpened() && !bLeftControlDown) // If the inventory is opened, and we don't press left control button, spawn the item on cursor
	{
		bool bDroppedItem = false;
		if(bIsDraggingAnItem)
		{
			bDroppedItem = HeroComp->DropItem();
		}

		if((!bIsDraggingAnItem) || (bIsDraggingAnItem && bDroppedItem))
		{
			HeroComp->ServerGrabDroppableItemToCursor(this);
			return true; // Added whole Item
		}
		return false; // Added some Item stacks
	}
	HeroComp->ServerPickupItemDef(this);
	
	return false; // Added whole Item
}

void AObsidianDroppableItem::DestroyItemDescription()
{
	if(ActiveItemDescription)
	{
		ActiveItemDescription->DestroyDescriptionWidget();
		ActiveItemDescription = nullptr;
	}
}



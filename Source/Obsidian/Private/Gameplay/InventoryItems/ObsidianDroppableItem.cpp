// Copyright 2024 out of sCope team - Michał Ogiński


#include "Gameplay/InventoryItems/ObsidianDroppableItem.h"
#include "CharacterComponents/ObsidianHeroComponent.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Components/WidgetComponent.h"
#include "InventoryItems/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "UI/Inventory/ObsidianDraggedItem.h"
#include "Kismet/GameplayStatics.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/Inventory/ObsidianItemWorldName.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "UI/ObsidianHUD.h"

AObsidianDroppableItem::AObsidianDroppableItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMeshComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	StaticMeshComp->SetCustomDepthStencilValue(ObsidianHighlight::White);
	StaticMeshComp->SetRenderCustomDepth(false);
	StaticMeshComp->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
	SetRootComponent(StaticMeshComp);
	
	GroundItemDescWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("GoundItemDescWidgetComp"));
	GroundItemDescWidgetComp->SetupAttachment(GetRootComponent());
	GroundItemDescWidgetComp->SetRelativeLocation(FVector(0.0f, 50.0f, 0.0f));
	GroundItemDescWidgetComp->SetDrawAtDesiredSize(true);
	GroundItemDescWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	GroundItemDescWidgetComp->SetupAttachment(StaticMeshComp);
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

void AObsidianDroppableItem::SetupItemAppearanceFromInstance()
{
	FPickupContent PickupContent = GetPickupContent();
	if(PickupContent.Instances.Num() > 0)
	{
		return;
	}
	
	if(const UObsidianInventoryItemInstance* ItemInstance = PickupContent.Instances[0].Item)
	{
		if(UStaticMesh* DroppedMesh = ItemInstance->GetItemDroppedMesh())
		{
			StaticMeshComp->SetStaticMesh(DroppedMesh);
		}
	}
}

void AObsidianDroppableItem::SetupItemAppearanceFromDefinition()
{
	FPickupContent PickupContent = GetPickupContent();
	if(PickupContent.Templates.Num() > 0)
	{
		return;
	}

	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = PickupContent.Templates[0].ItemDef;
	if(ItemDef == nullptr)
	{
		return;
	}

	if(const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef))
	{
		if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
		{
			if(UStaticMesh* DroppedMesh = AppearanceFrag->GetItemDroppedMesh())
			{
				StaticMeshComp->SetStaticMesh(DroppedMesh);	
			}
		}
	}
}

void AObsidianDroppableItem::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}
	
	UObsidianItemWorldName* GroundItemDesc = CreateWidget<UObsidianItemWorldName>(World, GroundItemDescClass);
	GroundItemDesc->OnItemDescMouseHoverDelegate.AddUObject(this, &ThisClass::OnItemDescMouseHover);
	GroundItemDesc->OnItemDescMouseButtonDownDelegate.AddUObject(this, &ThisClass::OnItemDescMouseButtonDown);

	InitItemDesc(GroundItemDesc);
	GroundItemDescWidgetComp->SetWidget(GroundItemDesc);
	GroundItemDescWidgetComp->InitWidget();
}

void AObsidianDroppableItem::InitItemDesc(UObsidianItemWorldName* GroundItemDesc)
{
	FPickupContent PickupContent = GetPickupContent();
	if(CarriesItemDef())
	{
		if(const TSubclassOf<UObsidianInventoryItemDefinition> PickupItemDef = PickupContent.Templates[0].ItemDef)
		{
			if(const UObsidianInventoryItemDefinition* DefaultItem = PickupItemDef.GetDefaultObject())
			{
				const UOInventoryItemFragment_Appearance* Appearance = Cast<UOInventoryItemFragment_Appearance>(DefaultItem->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass()));

				const FText ItemDisplayName = Appearance->GetItemDisplayName();
				GroundItemDesc->SetItemName(ItemDisplayName);
			}
		}
	}
	else if(CarriesItemInstance())
	{
		if(const UObsidianInventoryItemInstance* ItemInstance = PickupContent.Instances[0].Item)
		{
			const FText ItemDisplayName = ItemInstance->GetItemDisplayName();
			GroundItemDesc->SetItemName(ItemDisplayName);
		}
	}
}

void AObsidianDroppableItem::OnItemDescMouseHover(const bool bMouseEnter)
{
	if(StaticMeshComp == nullptr)
	{
		return;
	}

	int32 StackCount = -1;
	if(CarriesItemDef())
	{
		StackCount = GetPickupContent().Templates[0].StackCount;
	}
	if(CarriesItemInstance())
	{
		StackCount = GetPickupContent().Instances[0].Item->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	}
	UE_LOG(LogTemp, Error, TEXT("Item Stacks: [%d]"), StackCount);

	StaticMeshComp->SetRenderCustomDepth(bMouseEnter);
}

void AObsidianDroppableItem::OnItemDescMouseButtonDown(const bool bLeftControlDown)
{
	//TODO For the future
	// 1. Get the local Player Controller
	// 2. Check if the inventory is opened
	// 3. Server delegate to request to add the item to inventory/cursor?
	bool bAddedWholeItem = true;
	if(CarriesItemDef())
	{
		bAddedWholeItem = PickupItemDef(bLeftControlDown);
	}
	else if(CarriesItemInstance())
	{
		bAddedWholeItem = PickupItemInstance(bLeftControlDown);
	}
	
	if(bAddedWholeItem)
	{
		Destroy();
	}
}

bool AObsidianDroppableItem::PickupItemInstance(const bool bLeftControlDown)
{
	AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if(ObsidianPC == nullptr)
	{
		return false;
	}
	AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD();
	if(ObsidianHUD == nullptr)
	{
		return false;
	}
		
	FPickupContent PickupContent = GetPickupContent();
	UObsidianInventoryItemInstance* ItemInstance = PickupContent.Instances[0].Item;
	
	const AActor* OwningActor = Cast<AActor>(ObsidianPC->GetPawn());
	UObsidianHeroComponent* HeroComp = UObsidianHeroComponent::FindHeroComponent(OwningActor);
	check(HeroComp);
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
			UObsidianDraggedItem* DraggedItem = CreateWidget<UObsidianDraggedItem>(ObsidianPC, DraggedItemWidgetClass);
			DraggedItem->InitializeItemWidgetWithItemInstance(ItemInstance);
			DraggedItem->AddToViewport();
			HeroComp->DragItem(DraggedItem);
			return true;
		}
		return false;
	}
	
	int32 OutStacksLeft = 0;
	UObsidianInventoryComponent* InventoryComponent = ObsidianPC->GetInventoryComponent();
	InventoryComponent->AddItemInstance(ItemInstance, /** OUT */ OutStacksLeft);
			
	if(OutStacksLeft > 0)
	{
		ItemInstance->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, OutStacksLeft);
		return false;
	}
	return true;
}

bool AObsidianDroppableItem::PickupItemDef(const bool bLeftControlDown)
{
	AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if(ObsidianPC == nullptr)
	{
		return false;
	}

	AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD();
	if(ObsidianHUD == nullptr)
	{
		return false;
	}

	FPickupContent PickupContent = GetPickupContent();
	const TSubclassOf<UObsidianInventoryItemDefinition> PickupItemDef = PickupContent.Templates[0].ItemDef;
	const int32 StackCount = PickupContent.Templates[0].StackCount;

	const AActor* OwningActor = Cast<AActor>(ObsidianPC->GetPawn());
	UObsidianHeroComponent* HeroComp = UObsidianHeroComponent::FindHeroComponent(OwningActor);
	check(HeroComp);
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
			UObsidianDraggedItem* DraggedItem = CreateWidget<UObsidianDraggedItem>(ObsidianPC, DraggedItemWidgetClass);
			DraggedItem->InitializeItemWidgetWithItemDef(PickupItemDef, StackCount);
			DraggedItem->AddToViewport();
			HeroComp->DragItem(DraggedItem);
			return true;
		}
		return false;
	}
	
	UObsidianInventoryComponent* InventoryComponent = ObsidianPC->GetInventoryComponent();

	int32 OutStacksLeft = 0;
	InventoryComponent->AddItemDefinition(PickupItemDef, /** OUT */ OutStacksLeft, StackCount);
	if(OutStacksLeft > 0)
	{
		OverrideTemplateStacks(0, OutStacksLeft);
		return false;
	}
	return true;
}



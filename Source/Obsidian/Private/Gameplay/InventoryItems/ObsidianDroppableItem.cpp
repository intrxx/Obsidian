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
	
	WorldItemNameWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WorldItemNameWidgetComp"));
	WorldItemNameWidgetComp->SetupAttachment(GetRootComponent());
	WorldItemNameWidgetComp->SetRelativeLocation(FVector(0.0f, 50.0f, 0.0f));
	WorldItemNameWidgetComp->SetDrawAtDesiredSize(true);
	WorldItemNameWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	WorldItemNameWidgetComp->SetupAttachment(StaticMeshComp);
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
	const UObsidianInventoryItemInstance* ItemInstance = GetFirstItemInstanceFromPickupContent().Item;
	if(ItemInstance == nullptr)
	{
		return;
	}
	
	if(UStaticMesh* DroppedMesh = ItemInstance->GetItemDroppedMesh())
	{
		StaticMeshComp->SetStaticMesh(DroppedMesh);
		return;
	}
	
	UE_LOG(LogInventory, Error, TEXT("Item [%s] failed to set the Dropped Mesh in: AObsidianDroppableItem::SetupItemAppearanceFromDefinition"),
		*ItemInstance->GetItemDebugName());
}

void AObsidianDroppableItem::SetupItemAppearanceFromDefinition()
{
	const FPickupTemplate PickupTemplate = GetFirstItemDefFromPickupContent();
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = PickupTemplate.ItemDef;
	if(ItemDef == nullptr)
	{
		return;
	}
	
	const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef);
	if(ItemDefault == nullptr)
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
	
	UE_LOG(LogInventory, Error, TEXT("Item [%s] failed to set the Dropped Mesh in: AObsidianDroppableItem::SetupItemAppearanceFromDefinition"),
		*ItemDefault->GetDebugName());
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
	GroundItemDesc->OnItemWorldNameMouseHoverDelegate.AddUObject(this, &ThisClass::OnItemWorldNameMouseHover);
	GroundItemDesc->OnItemWorldNameMouseButtonDownDelegate.AddUObject(this, &ThisClass::OnItemWorldNameMouseButtonDown);
	InitItemDesc(GroundItemDesc);
	
	WorldItemNameWidgetComp->SetWidget(GroundItemDesc);
	WorldItemNameWidgetComp->InitWidget();
}

void AObsidianDroppableItem::InitItemDesc(UObsidianItemWorldName* GroundItemDesc)
{
	if(const TSubclassOf<UObsidianInventoryItemDefinition> PickupItemDef = GetFirstItemDefFromPickupContent().ItemDef)
	{
		if(const UObsidianInventoryItemDefinition* DefaultItem = PickupItemDef.GetDefaultObject())
		{
			const UOInventoryItemFragment_Appearance* Appearance = Cast<UOInventoryItemFragment_Appearance>(DefaultItem->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass()));

			const FText ItemDisplayName = Appearance->GetItemDisplayName();
			GroundItemDesc->SetItemName(ItemDisplayName);
		}
	}
	else if(const UObsidianInventoryItemInstance* ItemInstance = GetFirstItemInstanceFromPickupContent().Item)
    {
         const FText ItemDisplayName = ItemInstance->GetItemDisplayName();
         GroundItemDesc->SetItemName(ItemDisplayName);
    }
}

void AObsidianDroppableItem::OnItemWorldNameMouseHover(const bool bMouseEnter)
{
	if(StaticMeshComp)
	{
		StaticMeshComp->SetRenderCustomDepth(bMouseEnter);
	}

	if(bMouseEnter)
	{
		//TODO Display Item's Description and add it to the predefined place in the viewport
	}
	else
	{
		//TODO Destroy the widget? Maybe hide it and destroy it later?
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
}

void AObsidianDroppableItem::OnItemWorldNameMouseButtonDown(const bool bLeftControlDown)
{
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

	UObsidianInventoryItemInstance* ItemInstance = GetFirstItemInstanceFromPickupContent().Item;
	if(ItemInstance == nullptr)
	{
		return false;
	}
	
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

	const FPickupTemplate PickupTemplate = GetFirstItemDefFromPickupContent();
	const TSubclassOf<UObsidianInventoryItemDefinition> PickupItemDef = PickupTemplate.ItemDef;
	const int32 StackCount = PickupTemplate.StackCount;

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



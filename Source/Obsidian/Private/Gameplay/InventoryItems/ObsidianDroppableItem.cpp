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
#include "UI/Inventory/ObsidianGroundItemDesc.h"
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

void AObsidianDroppableItem::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}
	
	UObsidianGroundItemDesc* GroundItemDesc = CreateWidget<UObsidianGroundItemDesc>(World, GroundItemDescClass);
	GroundItemDesc->OnItemDescMouseHoverDelegate.AddUObject(this, &ThisClass::OnItemDescMouseHover);
	GroundItemDesc->OnItemDescMouseButtonDownDelegate.AddUObject(this, &ThisClass::OnItemDescMouseButtonDown);

	InitItemDesc(GroundItemDesc);
	GroundItemDescWidgetComp->SetWidget(GroundItemDesc);
	GroundItemDescWidgetComp->InitWidget();
}

void AObsidianDroppableItem::InitItemDesc(UObsidianGroundItemDesc* GroundItemDesc)
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

	StaticMeshComp->SetRenderCustomDepth(bMouseEnter);
}

void AObsidianDroppableItem::OnItemDescMouseButtonDown()
{
	//TODO For the future
	// 1. Get the local Player Controller
	// 2. Check if the inventory is opened
	// 3. Server delegate to request to add the item to inventory/cursor?
	if(CarriesItemDef())
	{
		PickupItemDef();
	}
	else if(CarriesItemInstance())
	{
		PickupItemInstance();
	}
	
	Destroy();
}

void AObsidianDroppableItem::PickupItemInstance() const
{
	if(AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		if(AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD())
		{
			FPickupContent PickupContent = GetPickupContent();
			UObsidianInventoryItemInstance* ItemInstance = PickupContent.Instances[0].Item;
			
			if(ObsidianHUD->IsInventoryOpened()) // If the inventory is opened spawn the item on cursor
			{
				const AActor* OwningActor = Cast<AActor>(ObsidianPC->GetPawn());
				UObsidianHeroComponent* HeroComp = UObsidianHeroComponent::FindHeroComponent(OwningActor);
				check(HeroComp);
					
				UObsidianDraggedItem* DraggedItem = CreateWidget<UObsidianDraggedItem>(ObsidianPC, DraggedItemWidgetClass);
				DraggedItem->InitializeItemWidgetWithItemInstance(ItemInstance);
				DraggedItem->AddToViewport();
				HeroComp->DragItem(DraggedItem);
				return;
			}
			UObsidianInventoryComponent* InventoryComponent = ObsidianPC->GetInventoryComponent();
			InventoryComponent->AddItemInstance(ItemInstance);
		}
	}
}

void AObsidianDroppableItem::PickupItemDef() const
{
	if(AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		if(AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD())
		{
			FPickupContent PickupContent = GetPickupContent();
			const TSubclassOf<UObsidianInventoryItemDefinition> PickupItemDef = PickupContent.Templates[0].ItemDef;
			const int32 StackCount = PickupContent.Templates[0].StackCount;
			
			if(ObsidianHUD->IsInventoryOpened()) // If the inventory is opened spawn the item on cursor
			{
				const AActor* OwningActor = Cast<AActor>(ObsidianPC->GetPawn());
				UObsidianHeroComponent* HeroComp = UObsidianHeroComponent::FindHeroComponent(OwningActor);
				check(HeroComp);
					
				UObsidianDraggedItem* DraggedItem = CreateWidget<UObsidianDraggedItem>(ObsidianPC, DraggedItemWidgetClass);
				DraggedItem->InitializeItemWidgetWithItemDef(PickupItemDef, StackCount);
				DraggedItem->AddToViewport();
				HeroComp->DragItem(DraggedItem);
				return;
			}
			UObsidianInventoryComponent* InventoryComponent = ObsidianPC->GetInventoryComponent();
			InventoryComponent->AddItemDefinition(PickupItemDef, StackCount);
		}
	}
}



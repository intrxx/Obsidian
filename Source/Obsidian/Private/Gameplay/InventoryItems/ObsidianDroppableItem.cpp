// Copyright 2024 out of sCope team - Michał Ogiński


#include "Gameplay/InventoryItems/ObsidianDroppableItem.h"
#include "CharacterComponents/ObsidianHeroComponent.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Components/WidgetComponent.h"
#include "InventoryItems/ObsidianInventoryComponent.h"
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

	FPickupContent PickupContent = GetPickupContent();
	if(const TSubclassOf<UObsidianInventoryItemDefinition> PickupItemDef = PickupContent.Templates[0].ItemDef)
	{
		if(const UObsidianInventoryItemDefinition* DefaultItem = PickupItemDef.GetDefaultObject())
		{
			const UOInventoryItemFragment_Appearance* Appearance = Cast<UOInventoryItemFragment_Appearance>(DefaultItem->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass()));

			const FText ItemDisplayName = Appearance->GetItemDisplayName();
			GroundItemDesc->SetItemName(ItemDisplayName);
		}
	}

	GroundItemDescWidgetComp->SetWidget(GroundItemDesc);
	GroundItemDescWidgetComp->InitWidget();
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
	if(AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		// Debug
		ENetRole NetRole = GetLocalRole();
		UE_LOG(LogTemp, Warning, TEXT("Role: %d"), NetRole);
		// Debug
		if(AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD())
		{
			FPickupContent PickupContent = GetPickupContent();
			const TSubclassOf<UObsidianInventoryItemDefinition> PickupItemDef = PickupContent.Templates[0].ItemDef;
			const int32 StackCount = PickupContent.Templates[0].StackCount;
			
			if(ObsidianHUD->IsInventoryOpened()) // If the inventory is opened spawn the item on cursor
			{
				AActor* OwningActor = Cast<AActor>(ObsidianPC->GetPawn());
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

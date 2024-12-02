// Copyright 2024 out of sCope team - Michał Ogiński


#include "Gameplay/InventoryItems/ObsidianDroppableItem.h"
#include "UI/Inventory/ObsidianItemDragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Components/WidgetComponent.h"
#include "InventoryItems/ObsidianInventoryComponent.h"
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
	
	ItemNameWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemNameWidgetComponent"));
	ItemNameWidgetComp->SetupAttachment(GetRootComponent());
	ItemNameWidgetComp->SetRelativeLocation(FVector(0.0f, 50.0f, 0.0f));
	ItemNameWidgetComp->SetDrawAtDesiredSize(true);
	ItemNameWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	ItemNameWidgetComp->SetupAttachment(StaticMeshComp);
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

	ItemNameWidgetComp->SetWidget(GroundItemDesc);
	ItemNameWidgetComp->InitWidget();
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
	//TODO Temp
	if(AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		// Debug
		ENetRole NetRole = GetLocalRole();
		UE_LOG(LogTemp, Warning, TEXT("Role: %d"), NetRole);

		if(AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD())
		{
			FPickupContent PickupContent = GetPickupContent();
			const TSubclassOf<UObsidianInventoryItemDefinition> PickupItemDef = PickupContent.Templates[0].ItemDef;
			const int32 StackCount = PickupContent.Templates[0].StackCount;
			
			if(ObsidianHUD->IsInventoryOpened()) // If the inventory is opened spawn the item on cursor
			{
				UObsidianDraggedItem* DraggedItem = CreateWidget<UObsidianDraggedItem>(ObsidianPC, DraggedItemWidgetClass);
				DraggedItem->InitializeItemWidget(PickupItemDef, StackCount);
				DraggedItem->AddToViewport();

				// Maybe create widget component on Players cursor and add the widget to the widget component?
				// Explore some way to maybe create a tickable subsystem to control the widget? PC seems better tho.
				// Maybe take it from Pc to Hero comp.

				UDragDropOperation* DragDropOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(UObsidianItemDragDropOperation::StaticClass());
				DragDropOperation->DefaultDragVisual = DraggedItem;
				
				return;
			}
			
			UObsidianInventoryComponent* InventoryComponent = ObsidianPC->GetInventoryComponent();
			InventoryComponent->AddItemDefinition(PickupItemDef, StackCount);
		}
	}
}

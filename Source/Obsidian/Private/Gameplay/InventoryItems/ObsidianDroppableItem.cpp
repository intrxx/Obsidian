// Copyright 2024 out of sCope team - Michał Ogiński


#include "Gameplay/InventoryItems/ObsidianDroppableItem.h"

#include "Characters/Player/ObsidianPlayerController.h"
#include "Components/WidgetComponent.h"
#include "InventoryItems/ObsidianInventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Inventory/ObsidianGroundItemDesc.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"

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
	AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if(ObsidianPC)
	{
		ENetRole NetRole = GetLocalRole();
		UE_LOG(LogTemp, Warning, TEXT("Role: %d"), NetRole);
		
		UObsidianInventoryComponent* InventoryComponent = ObsidianPC->GetInventoryComponent();
		InventoryComponent->AddItemDefinition(GetPickupContent().Templates[0].ItemDef, GetPickupContent().Templates[0].StackCount);
	}
}

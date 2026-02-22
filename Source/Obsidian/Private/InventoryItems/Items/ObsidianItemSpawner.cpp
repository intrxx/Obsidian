// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Items/ObsidianItemSpawner.h"

#include "NavigationSystem.h"
#include "InventoryItems/ItemDrop/ObsidianItemDropComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"

AObsidianItemSpawner::AObsidianItemSpawner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;
	bNetLoadOnClient = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetCollisionResponseToChannel(Obsidian_TraceChannel_PlayerCursorTrace, ECR_Block);
	StaticMeshComp->SetCollisionResponseToChannel(Obsidian_ObjectChannel_Projectile, ECR_Block);
	StaticMeshComp->SetCustomDepthStencilValue(ObsidianHighlight::White);
	StaticMeshComp->SetRenderCustomDepth(false);
	SetRootComponent(StaticMeshComp);

	SpawnPointComp = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPointComp"));
	SpawnPointComp->SetupAttachment(StaticMeshComp);

	ItemDropComponent = CreateDefaultSubobject<UObsidianItemDropComponent>(TEXT("ItemDropComponent"));
}

void AObsidianItemSpawner::SpawnItem()
{
	if (ItemDropComponent)
	{
		ItemDropComponent->DropItems(ItemSpawnerRarity, GetItemSpawnerLevel(), GetItemSpawnLocation());
		ItemDropComponent->OnDroppingItemsFinishedDelegate.AddUObject(this, &ThisClass::OnSpawningItemsFinished);
	}
}

uint8 AObsidianItemSpawner::GetItemSpawnerLevel() const
{
	if (ItemSpawnerLevelPolicy == EObsidianItemSpawnerLevelPolicy::Static)
	{
		return ItemSpawnerStaticLevel;
	}
	if (ItemSpawnerLevelPolicy == EObsidianItemSpawnerLevelPolicy::InheritFromArea)
	{
		//TODO(intrxx) Get Area Level
		ensureMsgf(false, TEXT("InheritFromArea is unimplemented"));
		return 1;
	}
	
	return 0;
}

void AObsidianItemSpawner::OnSpawningItemsFinished(const bool bDroppedItem)
{
	++TimesDropped;
	
	if (TimesToDrop > 0)
	{
		if (TimesDropped == TimesToDrop)
		{
			bCanInteract = false;
		}
	}
}

AActor* AObsidianItemSpawner::GetHighlightAvatarActor()
{
	return this;
}

void AObsidianItemSpawner::StartHighlight()
{
	if(StaticMeshComp)
	{
		StaticMeshComp->SetRenderCustomDepth(true);
	}
}

void AObsidianItemSpawner::StopHighlight()
{
	if(StaticMeshComp)
	{
		StaticMeshComp->SetRenderCustomDepth(false);
	}
}

AActor* AObsidianItemSpawner::GetInteractionActor()
{
	return this;
}

bool AObsidianItemSpawner::CanInteract()
{
	return bCanInteract;
}

float AObsidianItemSpawner::GetInteractionRadius()
{
	return InteractionRadius;
}

void AObsidianItemSpawner::Interact(AObsidianPlayerController* InteractingPlayerController)
{
	if(InteractingPlayerController)
	{
		InteractingPlayerController->ServerSpawnItemFromSpawner(this);
	}
}

FVector AObsidianItemSpawner::GetItemSpawnLocation() const
{
	check(SpawnPointComp)
	if (SpawnPointComp)
	{
		if (bRandomizeDropLocation == false)
		{
			return SpawnPointComp->GetComponentLocation();
		}

		UWorld* World = GetWorld();
		if (World == nullptr)
		{
			return FVector::Zero();
		}

		if(const UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(World))
		{
			FNavLocation RandomPoint;
			if(NavigationSystem->GetRandomPointInNavigableRadius(SpawnPointComp->GetComponentLocation(),
				RandomizedLocationRadius, RandomPoint))
			{
				return RandomPoint.Location;
			}
		}
	}
	return FVector::Zero();
}





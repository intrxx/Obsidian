// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Items/ObsidianItemSpawner.h"

// ~ Core

// ~ Project
#include "InventoryItems/ItemDrop/ObsidianItemDropComponent.h"
#include "InventoryItems/Items/ObsidianDroppableItem.h"
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
	if (++TimesDropped == TimesToDrop)
	{
		bCanInteract = false;
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
	return SpawnPointComp != nullptr ? SpawnPointComp->GetComponentLocation() : FVector::Zero();
}





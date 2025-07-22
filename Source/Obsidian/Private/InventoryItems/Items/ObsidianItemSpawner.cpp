// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Items/ObsidianItemSpawner.h"

// ~ Core

// ~ Project
#include "InventoryItems/Items/ObsidianDroppableItem.h"
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
}

void AObsidianItemSpawner::ServerSpawnItem_Implementation()
{
	if(bRandomizeItem)
	{
		RollItemDrop();
	}
	
	if(UWorld* World = GetWorld())
	{
		AObsidianDroppableItem* Item = World->SpawnActorDeferred<AObsidianDroppableItem>(ItemToDropClass, GetActorTransform());

		const FVector SpawnLocation = GetItemSpawnLocation();
		const FTransform Transform(FQuat(FRotator(0.0f, 0.0f, 0.0f)),
			SpawnLocation, FVector(1.0f, 1.0f, 1.0f));
		
		Item->FinishSpawning(Transform);
		SpawnedItems++;

		if(SpawnedItems == MaxSpawnCount)
		{
			StaticMeshComp->OnClicked.Clear();
		}
	}
}

void AObsidianItemSpawner::RollItemDrop()
{
	
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

void AObsidianItemSpawner::Interact()
{
	ServerSpawnItem();
}

FVector AObsidianItemSpawner::GetItemSpawnLocation() const
{
	return SpawnPointComp != nullptr ? SpawnPointComp->GetComponentLocation() : FVector::Zero();
}





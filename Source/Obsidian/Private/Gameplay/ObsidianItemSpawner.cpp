// Copyright 2024 out of sCope team - Michał Ogiński

#include "Gameplay/ObsidianItemSpawner.h"

#include "Gameplay/InventoryItems/ObsidianDroppableItem.h"
#include "Kismet/GameplayStatics.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"

AObsidianItemSpawner::AObsidianItemSpawner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SetReplicates(true);
	bNetLoadOnClient = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetCustomDepthStencilValue(ObsidianHighlight::White);
	StaticMeshComp->SetRenderCustomDepth(false);
	StaticMeshComp->SetCollisionResponseToChannel(Obsidian_TraceChannel_PlayerCursorTrace, ECR_Block);
	StaticMeshComp->SetCollisionResponseToChannel(Obsidian_ObjectChannel_Projectile, ECR_Block);
	SetRootComponent(StaticMeshComp);

	SpawnPointComp = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPointComp"));
	SpawnPointComp->SetupAttachment(StaticMeshComp);
}

void AObsidianItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	if(StaticMeshComp)
	{
		StaticMeshComp->OnClicked.AddDynamic(this, &ThisClass::OnMeshClicked);
	}
}

void AObsidianItemSpawner::OnMeshClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	//TODO This probably will be replaced by cursor trace from the hero comp
	if(ButtonPressed != EKeys::LeftMouseButton)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Hi %s"), *GetNameSafe(TouchedComponent));
	ServerSpawnItem();
}

void AObsidianItemSpawner::ServerSpawnItem_Implementation()
{
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

FVector AObsidianItemSpawner::GetItemSpawnLocation() const
{
	return SpawnPointComp != nullptr ? SpawnPointComp->GetComponentLocation() : FVector::Zero();
}





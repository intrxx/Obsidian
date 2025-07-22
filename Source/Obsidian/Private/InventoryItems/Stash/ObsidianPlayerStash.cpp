// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Stash/ObsidianPlayerStash.h"

// ~ Core

// ~ Project
#include "ObsidianTypes/ObsidianCoreTypes.h"

AObsidianPlayerStash::AObsidianPlayerStash(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootSceneComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	SetRootComponent(RootSceneComponent);
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComp->SetCollisionResponseToChannel(Obsidian_TraceChannel_PlayerCursorTrace, ECR_Block);
	StaticMeshComp->SetCollisionResponseToChannel(Obsidian_ObjectChannel_Projectile, ECR_Block);
	StaticMeshComp->SetCustomDepthStencilValue(ObsidianHighlight::White);
	StaticMeshComp->SetRenderCustomDepth(false);
	StaticMeshComp->SetupAttachment(RootSceneComponent);
}

AActor* AObsidianPlayerStash::GetHighlightAvatarActor()
{
	return this;
}

void AObsidianPlayerStash::StartHighlight()
{
	if(StaticMeshComp)
	{
		StaticMeshComp->SetRenderCustomDepth(true);
	}
}

void AObsidianPlayerStash::StopHighlight()
{
	if(StaticMeshComp)
	{
		StaticMeshComp->SetRenderCustomDepth(false);
	}
}

AActor* AObsidianPlayerStash::GetInteractionActor()
{
	return this;
}

bool AObsidianPlayerStash::CanInteract()
{
	return bCanInteract;
}

float AObsidianPlayerStash::GetInteractionRadius()
{
	return InteractionRadius;
}

void AObsidianPlayerStash::Interact(AObsidianPlayerController* InteractingPlayerController)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("Opening Chest")));
}


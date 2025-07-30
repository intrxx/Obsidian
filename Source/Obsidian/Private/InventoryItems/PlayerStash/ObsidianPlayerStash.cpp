// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/PlayerStash//ObsidianPlayerStash.h"

// ~ Core

// ~ Project
#include "Characters/Player/ObsidianPlayerController.h"
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

bool AObsidianPlayerStash::RequiresOngoingInteraction()
{
	return true;
}

float AObsidianPlayerStash::GetInteractionRadius()
{
	return InteractionRadius;
}

void AObsidianPlayerStash::Interact(AObsidianPlayerController* InteractingPlayerController)
{
	if(InteractingPlayerController == nullptr)
	{
		return;
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("Opening Chest")));

	//TODO Play sound and stash animation

	InteractingPlayerController->TogglePlayerStash(true);
}

void AObsidianPlayerStash::StopInteraction(AObsidianPlayerController* InteractingPlayerController)
{
	if(InteractingPlayerController)
	{
		InteractingPlayerController->TogglePlayerStash(false);
	}
}


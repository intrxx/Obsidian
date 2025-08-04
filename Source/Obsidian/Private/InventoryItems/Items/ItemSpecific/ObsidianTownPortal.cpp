// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Items/ItemSpecific/ObsidianTownPortal.h"

// ~ Core
#include "Net/UnrealNetwork.h"

// ~ Project
#include "Characters/Player/ObsidianPlayerController.h"

AObsidianTownPortal::AObsidianTownPortal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;
}

void AObsidianTownPortal::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bCanTeleport);
}

void AObsidianTownPortal::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if(HasAuthority() && World)
	{
		World->GetTimerManager().SetTimer(PortalOpenedTimerHandle, FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				bCanTeleport = true;
				bIsOpening = false;
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Portal has opened, you can teleport.")));
			}), PortalPrepareToOpenDuration, false);
	}
}

void AObsidianTownPortal::InitializePortal()
{
	PortalOwner = Cast<AObsidianPlayerController>(GetOwner());
	check(PortalOwner);

	bCanTeleport = false;
	bIsOpening = true;
}

bool AObsidianTownPortal::IsOpeningInProgress() const
{
	return bIsOpening;
}

AActor* AObsidianTownPortal::GetInteractionActor()
{
	return this;
}

bool AObsidianTownPortal::CanInteract()
{
	return bCanTeleport;
}

float AObsidianTownPortal::GetInteractionRadius()
{
	return InteractionRadius;
}

void AObsidianTownPortal::Interact(AObsidianPlayerController* InteractingPlayerController)
{
	if(InteractingPlayerController == nullptr)
	{
		return;
	}

	if(UWorld* World = InteractingPlayerController->GetWorld())
	{
		// Temp
		World->ServerTravel(FString("/Game/Obsidian/Levels/TestLevels/L_Town_Test?listen"));
	}
}



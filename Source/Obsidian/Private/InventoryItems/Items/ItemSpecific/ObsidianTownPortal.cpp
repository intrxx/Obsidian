// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Items/ItemSpecific/ObsidianTownPortal.h"

#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "Characters/Player/ObsidianPlayerController.h"
#include "Obsidian/ObsidianLogCategories.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"


AObsidianTownPortal::AObsidianTownPortal(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

	ClickableMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TownClickableMeshComp"));
	ClickableMeshComponent->SetupAttachment(RootComponent);
	ClickableMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	ClickableMeshComponent->SetCollisionResponseToChannel(Obsidian_TraceChannel_PlayerCursorTrace, ECR_Block);
	ClickableMeshComponent->SetVisibility(false);
	
	PortalNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalNiagaraComponent"));
	PortalNiagaraComponent->SetupAttachment(RootComponent);
	PortalNiagaraComponent->SetAutoDestroy(true);
}

void AObsidianTownPortal::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bCanTeleport);
}

void AObsidianTownPortal::BeginPlay()
{
	Super::BeginPlay();

	StartPortalOpeningTimer();

	if(ensureMsgf(PortalOpeningEffect, TEXT("Portal Opening Effect is invalid in ObsidianTownPortal instance, please fill it.")))
	{
		PortalNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PortalOpeningEffect, GetActorLocation(), GetActorRotation());
	}
}

void AObsidianTownPortal::StartPortalOpeningTimer()
{
	UWorld* World = GetWorld();
	if(HasAuthority() && World)
	{
		World->GetTimerManager().SetTimer(PortalOpenedTimerHandle, FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				UE_LOG(ObLogItems, Verbose, TEXT("Portal has opened, you can teleport."));

				bCanTeleport = true;
				bIsOpening = false;
			
				if(PortalNiagaraComponent && ensureMsgf(PortalEffect, TEXT("Portal Effect is invalid in ObsidianTownPortal instance, please fill it.")))
				{
					PortalNiagaraComponent->Deactivate();

					PortalNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PortalEffect, GetActorLocation(), GetActorRotation());
				}
			
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

void AObsidianTownPortal::Interact(AObsidianPlayerController* InInteractingPlayerController)
{
	if(InInteractingPlayerController == nullptr)
	{
		return;
	}

	if(UWorld* World = InInteractingPlayerController->GetWorld())
	{
		// Temp
		World->ServerTravel(FString("/Game/Obsidian/Levels/TestLevels/L_Town_Test?listen"));
	}
}



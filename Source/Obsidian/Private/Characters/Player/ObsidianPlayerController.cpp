// Copyright 2024 Michał Ogiński

#include "Characters/Player/ObsidianPlayerController.h"

#include "Interaction/ObsidianHighlightInterface.h"
#include "ObsidianTypes/ObsidianChannels.h"

AObsidianPlayerController::AObsidianPlayerController()
{
	bReplicates = true;
}

void AObsidianPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	CursorTrace();
}

void AObsidianPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(Obsidian_TraceChannel_Highlight, false, CursorHit);

	if(!CursorHit.bBlockingHit)
	{
		return;
	}

	LastHighlightedActor = CurrentHighlightedActor;
	CurrentHighlightedActor = Cast<IObsidianHighlightInterface>(CursorHit.GetActor());

	if(LastHighlightedActor == nullptr)
	{
		if(CurrentHighlightedActor)
		{
			CurrentHighlightedActor->StartHighlight();
		}
	}
	else // Last valid
	{
		if(CurrentHighlightedActor == nullptr)
		{
			LastHighlightedActor->StopHighlight();
		}
		else // Both valid
		{
			if(LastHighlightedActor != CurrentHighlightedActor)
			{
				LastHighlightedActor->StopHighlight();
				CurrentHighlightedActor->StartHighlight();
			}
		}
	}
}

void AObsidianPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	CurrentMouseCursor = EMouseCursor::Default;
	
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}



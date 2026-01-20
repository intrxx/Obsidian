// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_TownPortal.h"

// ~ Core

// ~ Project
#include "Characters/Heroes/ObsidianHero.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Game/ObsidianGameMode.h"
#include "InventoryItems/Items/ItemSpecific/ObsidianTownPortal.h"
#include "Kismet/GameplayStatics.h"

bool UObsidianUsableShard_TownPortal::OnItemUsed(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	if(ItemOwner == nullptr || UsingInstance == nullptr)
	{
		return false;
	}

	if(const AObsidianHero* ObsidianHero = ItemOwner->GetObsidianHero())
	{
		UWorld* World = ItemOwner->GetWorld();
		if(World == nullptr)
		{
			return false;
		}

		AObsidianGameMode* ObsidianGameMode = Cast<AObsidianGameMode>(UGameplayStatics::GetGameMode(World));
		if(ObsidianGameMode == nullptr)
		{
			return false;
		}

		if(ObsidianGameMode->CanCreatePortal())
		{
			const FVector DefaultSpawnLocation = ObsidianHero->GetActorLocation();
			FTransform PortalTransform = FTransform(DefaultSpawnLocation) + SpawnTransform;
			const FVector PortalDefaultLocation = PortalTransform.GetLocation();
				
			FHitResult GroundTraceResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(ObsidianHero);
			const FVector GroundTraceEndLocation = FVector(PortalDefaultLocation.X, PortalDefaultLocation.Y, PortalDefaultLocation.Z - 300.0f);
			World->LineTraceSingleByChannel(GroundTraceResult, PortalDefaultLocation, GroundTraceEndLocation, ECC_Visibility, QueryParams);
			if(GroundTraceResult.bBlockingHit)
			{
				PortalTransform.SetLocation(FVector(GroundTraceResult.Location.X, GroundTraceResult.Location.Y, GroundTraceResult.Location.Z + SpawnTransform.GetLocation().Z));
			}
			
			checkf(TownPortalActorClass, TEXT("TownPortalActorClass is not set on UObsidianUsableShard_TownPortal, please fill it."));
			AObsidianTownPortal* TownPortal = World->SpawnActorDeferred<AObsidianTownPortal>(TownPortalActorClass, PortalTransform, ItemOwner);
			TownPortal->InitializePortal();
			TownPortal->FinishSpawning(PortalTransform);

			ObsidianGameMode->RegisterPortal(TownPortal);
			return true;
		}		
	}
	return false;
}

// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_TownPortal.h"

// ~ Core

// ~ Project
#include "Characters/Heroes/ObsidianHero.h"
#include "Characters/Player/ObsidianPlayerController.h"

bool UObsidianUsableShard_TownPortal::OnItemUsed(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	if(ItemOwner && UsingInstance)
	{
		if(AObsidianHero* ObsidianHero = ItemOwner->GetObsidianHero())
		{
			UWorld* World = ItemOwner->GetWorld();
			if (World == nullptr)
			{
				return false;
			}
			
			ULevel* CurrentLevel = World->GetCurrentLevel();
			const FVector HeroLocation = ObsidianHero->GetActorLocation();

			DrawDebugSphere(World, HeroLocation, 32.0f, 16, FColor::Red);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT("Opening Portal in Level [%s]"), *GetNameSafe(CurrentLevel)));
		}
		return true;
	}
	return false;
}

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// ~ Project


#include "ObsidianPlayerStashComponent.generated.h"

/**
 * Primary Player Stash Component of Obsidian to be used by Players.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBSIDIAN_API UObsidianPlayerStashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObsidianPlayerStashComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/** Returns the Player Stash Component if one exists on the specified actor, will be nullptr otherwise */
	UFUNCTION(BlueprintPure, Category = "Obsidian|Inventory")
	static UObsidianPlayerStashComponent* FindPlayerStashComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UObsidianPlayerStashComponent>() : nullptr);
	}
};

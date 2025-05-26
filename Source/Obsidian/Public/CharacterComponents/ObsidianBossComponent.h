// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Components/ActorComponent.h"
#include "ObsidianBossComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnBossThresholdReached)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBSIDIAN_API UObsidianBossComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObsidianBossComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/** Returns the Boss Component if one exists on the specified actor, will be nullptr otherwise */
	UFUNCTION(BlueprintPure, Category = "Obsidian|EnemyAttributes")
	static UObsidianBossComponent* FindBossComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UObsidianBossComponent>() : nullptr);
	}

	FOnBossThresholdReached OnBossThresholdReached_75Delegate;
	FOnBossThresholdReached OnBossThresholdReached_50Delegate;
	FOnBossThresholdReached OnBossThresholdReached_25Delegate;

protected:
	virtual void BeginPlay() override;
	
};

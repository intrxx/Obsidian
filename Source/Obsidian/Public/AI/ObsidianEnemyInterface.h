// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "UObject/Interface.h"
#include "ObsidianEnemyInterface.generated.h"

/**
 * 
 */
UINTERFACE(MinimalAPI)
class UObsidianEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OBSIDIAN_API IObsidianEnemyInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetCombatTarget(AActor* InTarget);
	virtual void SetCombatTarget_Implementation(AActor* InTarget) {};

	/** Gets the Combat Target that is set before using Gameplay Ability. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetCombatTarget() const;
	virtual AActor* GetCombatTarget_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	EObsidianEntityRarity GetEnemyRarityFromOwner() const; 
	virtual EObsidianEntityRarity GetEnemyRarityFromOwner_Implementation() const { return EObsidianEntityRarity::None; }
};
 
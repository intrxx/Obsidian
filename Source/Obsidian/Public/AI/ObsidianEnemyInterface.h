// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


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

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetCombatTarget(AActor* InTarget);
	virtual void SetCombatTarget_Implementation(AActor* InTarget);

	/** Gets the Combat Target that is set before using Gameplay Ability. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetCombatTarget() const;
	virtual AActor* GetCombatTarget_Implementation() const;
};

// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>


#include "AbilitySystem/Abilities/OGameplayAbility_ProjectileSpell.h"
#include "ObsidianGA_FlyingKnifes.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianGA_FlyingKnifes : public UOGameplayAbility_ProjectileSpell
{
	GENERATED_BODY()

public:
	UObsidianGA_FlyingKnifes(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Obsidian|FlyingKnifes")
	void FireFlyingKnifeProjectiles();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|FlyingKnifes")
	float SpawnRadius = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|FlyingKnifes")
	float SpawnHeight = 50.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|FlyingKnifes")
	uint8 NumberOfProjectiles = 4;
};

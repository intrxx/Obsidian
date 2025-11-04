// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>


#include "AbilitySystem/Abilities/OGameplayAbility_ProjectileSpell.h"
#include "ObGameplayAbility_FlyingKnife.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObGameplayAbility_FlyingKnife : public UOGameplayAbility_ProjectileSpell
{
	GENERATED_BODY()

public:
	UObGameplayAbility_FlyingKnife(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

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

// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/OGameplayAbility_ProjectileSpell.h"
#include "ObsidianGA_MagneticHammer.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianGA_MagneticHammer : public UOGameplayAbility_ProjectileSpell
{
	GENERATED_BODY()

public:
	UObsidianGA_MagneticHammer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void FireMagneticHammer(const FVector& TowardsTarget);
};

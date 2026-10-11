// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "Characters/Enemies/ObsidianRegularEnemy.h"

#include "ObsidianRegular_RangedGoblin.generated.h"

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianRegular_RangedGoblin : public AObsidianRegularEnemy
{
	GENERATED_BODY()

public:
	AObsidianRegular_RangedGoblin(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	//~ Begin UObject Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface

	//~ Begin IObsidianCombatInterface Interface
	virtual FVector GetAbilitySocketLocationFromLHWeapon_Implementation() override;
	//~ End IObsidianCombatInterface Interface

protected:
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Obsidian|Combat")
	TObjectPtr<USkeletalMeshComponent> LeftHandEquipmentMesh;
};

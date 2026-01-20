// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Characters/Enemies/ObsidianRegularEnemy.h"
#include "ObsidianRegular_RangedGoblin.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianRegular_RangedGoblin : public AObsidianRegularEnemy
{
	GENERATED_BODY()

public:
	AObsidianRegular_RangedGoblin(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	//~ Start of CombatInterface
	virtual FVector GetAbilitySocketLocationFromLHWeapon_Implementation() override;
	//~ End of CombatInterface
	
protected:
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Obsidian|Combat")
	TObjectPtr<USkeletalMeshComponent> LeftHandEquipmentMesh;
};

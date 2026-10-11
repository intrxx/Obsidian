// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "Characters/Enemies/ObsidianBossEnemy.h"

#include "ObsidianBoss_TreeOrc.generated.h"

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianBoss_TreeOrc : public AObsidianBossEnemy
{
	GENERATED_BODY()

public:
	AObsidianBoss_TreeOrc(const FObjectInitializer& InObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Obsidian|TreeOrc")
	void EquipWeapon();

	//~ Begin AActor Interface
	virtual void PostInitializeComponents() override;
	//~ End AActor Interface

	//~ Begin UObject Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface

	//~ Begin IObsidianCombatInterface Interface
	virtual FVector GetAbilitySocketLocationFromRHWeapon_Implementation() override;
	//~ End IObsidianCombatInterface Interface

protected:
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Obsidian|Combat")
	TObjectPtr<USkeletalMeshComponent> RightHandEquipmentMesh;

private:
	void HandleThreshold_50();

	FDelegateHandle OnThresholdReached_50DelegateHandle;
};

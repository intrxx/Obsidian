// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Characters/Enemies/ObsidianBossEnemy.h"
#include "ObsidianBoss_TreeOrc.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianBoss_TreeOrc : public AObsidianBossEnemy
{
	GENERATED_BODY()

public:
	AObsidianBoss_TreeOrc(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|TreeOrc")
	void EquipWeapon();

	//~ Start of CombatInterface
	virtual FVector GetAbilitySocketLocationFromRHWeapon_Implementation() override;
	//~ End of CombatInterface
	
protected:
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Obsidian|Combat")
	TObjectPtr<USkeletalMeshComponent> RightHandEquipmentMesh;
	
private:
	void HandleThreshold_50();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Obsidian|AI")
	TObjectPtr<UBehaviorTree> ArmedBehaviorTree;
	
	FDelegateHandle OnThresholdReached_50DelegateHandle;
};

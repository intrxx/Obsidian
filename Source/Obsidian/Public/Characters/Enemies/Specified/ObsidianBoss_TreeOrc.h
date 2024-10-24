// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
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

	UFUNCTION(BlueprintCallable, Category = "Obsidian|TreeOrc")
	void EquipWeapon();

private:
	void HandleThreshold_50();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Obsidian|AI")
	TObjectPtr<UBehaviorTree> ArmedBehaviorTree;
	
	FDelegateHandle OnThresholdReached_50DelegateHandle;
};

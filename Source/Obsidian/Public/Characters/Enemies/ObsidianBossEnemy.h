// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Characters/Enemies/ObsidianEnemy.h"
#include "ObsidianBossEnemy.generated.h"

class UObsidianBossComponent;
class UObsidianAdvancedCombatComponent;
class AObsidianHero;
class UBlackboardComponent;
class AObsidianAIControllerBase;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianBossEnemy : public AObsidianEnemy
{
	GENERATED_BODY()

public:
	AObsidianBossEnemy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "Handle Advanced Combat Hit"), Category = "Obsidian|Boss")
	void BP_HandleAdvancedCombatHit(const FHitResult& HitResult);

protected:
	//~ Start of AObsidianCharacterBase
	virtual void OnAbilitySystemInitialized() override;
	
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor) override;

	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor) override;
	//~ End of AObsidianCharacterBase

	UFUNCTION()
	void HandleAdvancedCombatHit(const FHitResult& HitResult);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianBossComponent> BossComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianAdvancedCombatComponent> AdvancedCombatComponent;
	
	UPROPERTY()
	TObjectPtr<AObsidianAIControllerBase> ObsidianBossAIController;
	
};

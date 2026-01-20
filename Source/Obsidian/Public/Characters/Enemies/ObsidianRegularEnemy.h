// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Characters/Enemies/ObsidianEnemy.h"
#include "ObsidianRegularEnemy.generated.h"

class AObsidianAIControllerBase;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianRegularEnemy : public AObsidianEnemy
{
	GENERATED_BODY()

public:
	AObsidianRegularEnemy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PossessedBy(AController* NewController) override;
	
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	AObsidianAIControllerBase* GetObsidianAIController() const
	{
		return ObsidianRegularAIController;
	}

protected:
	//~ Start of AObsidianCharacterBase
	virtual void OnAbilitySystemInitialized() override;
	
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor) override;

	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor) override;
	//~ End of AObsidianCharacterBase

private:
	void CreateHealthBarWidget() const;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> HealthBarWidgetComp;

	UPROPERTY()
	TObjectPtr<AObsidianAIControllerBase> ObsidianRegularAIController;
};

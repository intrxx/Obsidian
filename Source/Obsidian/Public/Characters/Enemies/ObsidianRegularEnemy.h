// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

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
	AObsidianRegularEnemy(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	virtual void PossessedBy(AController* InNewController) override;
	
	void HitReactTagChanged(const FGameplayTag InCallbackTag, int32 InNewCount);

	AObsidianAIControllerBase* GetObsidianAIController() const
	{
		return ObsidianRegularAIController;
	}

protected:
	//~ Start of AObsidianCharacterBase
	virtual void OnAbilitySystemInitialized() override;
	
	UFUNCTION()
	virtual void OnDeathStarted(AActor* InOwningActor) override;

	UFUNCTION()
	virtual void OnDeathFinished(AActor* InOwningActor) override;
	//~ End of AObsidianCharacterBase

private:
	void CreateHealthBarWidget() const;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> HealthBarWidgetComp;

	UPROPERTY()
	TObjectPtr<AObsidianAIControllerBase> ObsidianRegularAIController;
};

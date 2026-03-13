// Copyright 2026 out of sCope - intrxx

#pragma once

#include <CoreMinimal.h>

#include <GameplayTagContainer.h>

#include "CharacterComponents/Movement/ObsidianCharacterMovementComponent.h"
#include "ObsidianHeroMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianHeroMovementComponent : public UObsidianCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UObsidianHeroMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
		
	virtual float GetMaxSpeed() const override;

	FGameplayTag GetCurrentWalkState() const;
	bool IsInWalkingState() const;
	void ToggleWalkingState();
	
	void HandleOutOfStamina();

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool HandleWalkingStateChanged(const FGameplayTag& NewWalkingState);

protected:
	UPROPERTY(Replicated)
	FGameplayTag CurrentWalkState = FGameplayTag::EmptyTag;

	UPROPERTY(Replicated)
	uint8 bUserTurnOnWalkingState:1 = false;

	UPROPERTY(Replicated)
	uint8 bWentOutOfStamina:1 = false;
};

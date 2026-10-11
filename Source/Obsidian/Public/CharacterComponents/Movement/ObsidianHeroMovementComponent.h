// Copyright 2026 out of sCope - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

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
	UObsidianHeroMovementComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	FGameplayTag GetCurrentWalkState() const { return CurrentWalkState; }
	bool IsInWalkingState() const { return bUserTurnOnWalkingState; }
	void ToggleWalkingState();

	void HandleOutOfStamina();

	//~ Begin UObject Interface
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface

	//~ Begin UMovementComponent Interface
	virtual float GetMaxSpeed() const override;
	//~ End UMovementComponent Interface

	//~ Begin UActorComponent Interface
	virtual void TickComponent(float InDeltaTime, enum ELevelTick InTickType, FActorComponentTickFunction* InThisTickFunction) override;
	//~ End UActorComponent Interface

protected:
	bool HandleWalkingStateChanged(const FGameplayTag& InNewWalkingState);

	UPROPERTY(Replicated)
	FGameplayTag CurrentWalkState = FGameplayTag::EmptyTag;

	UPROPERTY(Replicated)
	uint8 bUserTurnOnWalkingState:1 = false;

	UPROPERTY(Replicated)
	uint8 bWentOutOfStamina:1 = false;
};

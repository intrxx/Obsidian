// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/PawnComponent.h"
#include "ObsidianHeroComponent.generated.h"

class AObsidianHUD;
struct FInputActionValue;

/**
 * Component that manages hero related things like input
 */
UCLASS()
class OBSIDIAN_API UObsidianHeroComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	UObsidianHeroComponent(const FObjectInitializer& ObjectInitializer);
	
	void InitializePlayerInput(UInputComponent* InputComponent);

	/** Returns the hero component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Obsidian|HeroComp")
	static UObsidianHeroComponent* FindHeroComponent(const AActor* Actor) {return (Actor ? Actor->FindComponentByClass<UObsidianHeroComponent>() : nullptr);}

	AObsidianHUD* GetObsidianHUD() const;

public:
	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold = 0.3f;

protected:
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	
	void Input_MoveKeyboard(const FInputActionValue& InputActionValue);
	void Input_MoveStartedMouse();
	void Input_MoveTriggeredMouse();
	void Input_MoveReleasedMouse();
	void Input_ToggleCharacterStatus();

private:
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	
};


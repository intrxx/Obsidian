// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "ObsidianHeroComponent.generated.h"

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

protected:
	void Input_Move(const FInputActionValue& InputActionValue);
	
};
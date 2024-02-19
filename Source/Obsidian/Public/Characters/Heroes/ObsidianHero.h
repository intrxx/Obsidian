// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Characters/ObsidianCharacterBase.h"
#include "ObsidianHero.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UObsidianHeroComponent;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianHero : public AObsidianCharacterBase
{
	GENERATED_BODY()
public:
	AObsidianHero();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Hero", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianHeroComponent> HeroComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Hero", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Hero", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmComponent;
};

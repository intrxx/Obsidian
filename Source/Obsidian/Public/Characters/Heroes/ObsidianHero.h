// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Characters/ObsidianCharacterBase.h"
#include "ObsidianHero.generated.h"

class AObsidianPlayerController;
class UObsidianHeroAttributesComponent;
class AObsidianPlayerState;
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
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Hero")
	AObsidianPlayerState* GetObsidianPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Hero")
	AObsidianPlayerController* GetObsidianPlayerController() const;

	//~ Start of CombatInterface
	virtual int32 GetCharacterLevel() override;
	//~ End of CombatInterface

protected:
	//~ Start of AObsidianCharacterBase
	virtual void OnAbilitySystemInitialized() override;
	virtual void OnAbilitySystemUninitialized() override;
	//~ End of AObsidianCharacterBase

	void InitializeUI(UObsidianAbilitySystemComponent* ObsidianASC);
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Hero", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianHeroComponent> HeroComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Hero", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianHeroAttributesComponent> HeroAttributesComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Hero", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Hero", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmComponent;
};

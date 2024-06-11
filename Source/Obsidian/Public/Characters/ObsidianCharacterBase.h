// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "AbilitySystemInterface.h"
#include "CoreMinimal.h"
#include "Combat/ObsidianCombatInterface.h"
#include "GameFramework/Character.h"
#include "ObsidianCharacterBase.generated.h"

class UMotionWarpingComponent;
class UObsidianAbilitySystemComponent;
class UObsidianPawnExtensionComponent;

UCLASS(Abstract)
class OBSIDIAN_API AObsidianCharacterBase : public ACharacter, public IAbilitySystemInterface, public IObsidianCombatInterface
{
	GENERATED_BODY()

public:
	AObsidianCharacterBase();

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Character")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Set to true on hero characters, false by default. Indicates if ObsidianCharacter is a Player Character (Hero) or AI (Enemy). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Character")
	bool bIsPlayer = false;

protected:
	virtual void BeginPlay() override;

	/** Fired when this actor is initialized as the avatar of the Ability System - Should be overridden by the child classes. */
	virtual void OnAbilitySystemInitialized();

	/** Fired when this actor is removed as the avatar of the Ability System - Should be overridden by the child classes. */
	virtual void OnAbilitySystemUninitialized();

	//~ Start of CombatInterface
	virtual FVector GetAbilitySocketLocationFromLHWeapon() override;
	virtual FVector GetAbilitySocketLocationFromRHWeapon() override;
	virtual FVector GetAbilitySocketLocationFromLeftHand() override;
	virtual FVector GetAbilitySocketLocationFromRightHand() override;
	virtual FVector GetAbilityDefaultLocation() override;
	virtual void SetMotionWarpingFacingTarget_Implementation(const FName MotionWarpName, const FVector& FacingTarget) override;
	//~ End of CombatInterface

protected:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian|Character")
	TObjectPtr<UObsidianPawnExtensionComponent> PawnExtComp;
	
	UPROPERTY(EditAnywhere, Category = "Obsidian|Combat")
	TObjectPtr<USkeletalMeshComponent> RightHandEquipmentMesh;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Combat")
	TObjectPtr<USkeletalMeshComponent> LeftHandEquipmentMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComp;

	/**
	 * Sockets used mostly for combat reasons, spawning projectiles.
	 */
	
	UPROPERTY(EditAnywhere, Category = "Obsidian|Combat|Setup")
	FName WeaponSocketName = FName();
	
	UPROPERTY(EditAnywhere, Category = "Obsidian|Combat|Setup")
	FName RightHandSocketName = FName();
	
	UPROPERTY(EditAnywhere, Category = "Obsidian|Combat|Setup")
	FName LeftHandSocketName = FName();

	UPROPERTY(EditAnywhere, Category = "Obsidian|Combat|Setup")
	FName DefaultSocketName = FName();

	/**
	 *
	 */

	/** Offset for how much the ability spawn location is moved from the player character. */ 
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Combat")
	float DefaultAbilitySocketLocationOffset = 0.f;
};

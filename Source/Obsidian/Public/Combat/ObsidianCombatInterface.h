// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ObsidianCombatInterface.generated.h"

UINTERFACE(MinimalAPI)
class UObsidianCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OBSIDIAN_API IObsidianCombatInterface
{
	GENERATED_BODY()

public:
	virtual int32 GetCharacterLevel();

	/** Returns true if the owning character is dead. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Obsidian|CombatInterface")
	bool IsDeadOrDying() const;	
	virtual bool IsDeadOrDying_Implementation() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Obsidian|CombatInterface")
	AActor* GetAvatarActor();
	virtual AActor* GetAvatarActor_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Obsidian|CombatInterface")
	void SetMotionWarpingFacingTarget(const FName MotionWarpName, const FVector& FacingTarget);
	virtual void SetMotionWarpingFacingTarget_Implementation(const FName MotionWarpName, const FVector& FacingTarget);
	
	/*
	 * Socket Locations for spawning abilities.
	 */

	/** Gets the socket location from left hand weapon. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Obsidian|CombatInterface")
	FVector GetAbilitySocketLocationFromLHWeapon();
	virtual FVector GetAbilitySocketLocationFromLHWeapon_Implementation();
	
	/** Gets the socket location from right hand weapon. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Obsidian|CombatInterface")
	FVector GetAbilitySocketLocationFromRHWeapon();
	virtual FVector GetAbilitySocketLocationFromRHWeapon_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Obsidian|CombatInterface")
	FVector GetAbilitySocketLocationFromLeftHand();
	virtual FVector GetAbilitySocketLocationFromLeftHand_Implementation();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Obsidian|CombatInterface")
	FVector GetAbilitySocketLocationFromRightHand();
	virtual FVector GetAbilitySocketLocationFromRightHand_Implementation();

	/** Gets the default location for spawning ability which is slightly ahead the player character. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Obsidian|CombatInterface")
	FVector GetAbilityDefaultLocation();
	virtual FVector GetAbilityDefaultLocation_Implementation();
	
	/*
	 * 
	 */
};

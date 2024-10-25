// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ObsidianCombatInterface.generated.h"

class UNiagaraSystem;
struct FGameplayTag;

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

	/** Gets the blood effect of the character, might be null if that character has no blood. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Obsidian|CombatInterface")
	UNiagaraSystem* GetBloodEffect();
	virtual UNiagaraSystem* GetBloodEffect_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Obsidian|CombatInterface")
	void SetMotionWarpingFacingTarget(const FName MotionWarpName = FName("FacingTarget"), const FVector& FacingTarget = FVector(0.0f, 0.0f, 0.0f));
	virtual void SetMotionWarpingFacingTarget_Implementation(const FName MotionWarpName = FName("FacingTarget"), const FVector& FacingTarget = FVector(0.0f, 0.0f, 0.0f));
	
	/*
	 * Socket Locations for spawning abilities.
	 */

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Obsidian|CombatInterface")
	FVector GetAbilitySocketLocationForTag(const FGameplayTag& Tag);
	virtual FVector GetAbilitySocketLocationForTag_Implementation(const FGameplayTag& Tag);

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
	FVector GetAbilityBetweenHandsSocketLocation();
	virtual FVector GetAbilityBetweenHandsSocketLocation_Implementation();
	
	/*
	 * 
	 */
};

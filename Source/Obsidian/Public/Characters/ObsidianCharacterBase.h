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
	AObsidianCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Character")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

	/** Fired when this actor is initialized as the avatar of the Ability System - Should be overridden by the child classes. */
	virtual void OnAbilitySystemInitialized();

	/** Fired when this actor is removed as the avatar of the Ability System - Should be overridden by the child classes. */
	virtual void OnAbilitySystemUninitialized();

	/** Begins the death sequence for the character (disables collision, disables movement, etc...) */
	virtual void OnDeathStarted(AActor* OwningActor);

	/** Ends the death sequence for the character (detaches controller, destroys pawn, etc...) */
	virtual void OnDeathFinished(AActor* OwningActor);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName="On Death Finished"), Category = "Obsidian|Character")
	void BP_OnDeathFinished();

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Character")
	void Ragdoll() const;

	void DestroyDueToDeath();
	void UninitAndDestroy();

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

	/** Offset for how much the ability spawn location is moved from the player character. */ 
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Combat")
	float DefaultAbilitySocketLocationOffset = 0.f;

	/**
	 * Death
	 */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Death")
	TArray<TObjectPtr<UAnimMontage>> DeathMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Death")
	FName RagdollImpulseBone = FName();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Death")
	float RagdollImpulseStrength = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Death")
	bool bRagdollWithImpulse = true;
};

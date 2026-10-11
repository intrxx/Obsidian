// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "AbilitySystemInterface.h"
#include "CoreMinimal.h"
#include "ModularCharacter.h"

#include "Combat/ObsidianCombatInterface.h"

#include "ObsidianCharacterBase.generated.h"

struct FGameplayAbilitySpec;

class UMassAgentComponent;
class UNiagaraSystem;
class UMotionWarpingComponent;
class UObsidianAbilitySystemComponent;
class UObsidianPawnExtensionComponent;

/**
 * 
 */
UCLASS(Abstract, meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianCharacterBase : public AModularCharacter, public IAbilitySystemInterface, public IObsidianCombatInterface
{
	GENERATED_BODY()

public:
	AObsidianCharacterBase(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Character")
    UCapsuleComponent* GetCapsuleComp() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Character")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const;

	FGameplayAbilitySpec* GetFirstAbilitySpecForTag(const FGameplayTag& InAbilityTag) const;

	bool CanHitReact() const { return bCanHitReact; }

	//~ Begin IAbilitySystemInterface Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface Interface

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	//~ Begin IObsidianCombatInterface Interface
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FVector GetAbilitySocketLocationForTag_Implementation(UPARAM(meta=(Categories="GameplayEvent.AbilityMontage")) FGameplayTag InTag) override;
	virtual FVector GetAbilitySocketLocationFromLeftHand_Implementation() override;
	virtual FVector GetAbilitySocketLocationFromRightHand_Implementation() override;
	virtual FVector GetAbilityBetweenHandsSocketLocation_Implementation() override;
	virtual FVector GetAbilitySocketLocationFromLHWeapon_Implementation() override;
	virtual FVector GetAbilitySocketLocationFromRHWeapon_Implementation() override;
	virtual void SetMotionWarpingFacingTarget_Implementation(const FName InMotionWarpName = FName("FacingTarget"), const FVector& InFacingTarget = FVector(0.0f, 0.0f, 0.0f)) override;
	//~ End IObsidianCombatInterface Interface

protected:
	/** Fired when this actor is initialized as the avatar of the Ability System - Should be overridden by the child classes. */
	virtual void OnAbilitySystemInitialized();

	/** Fired when this actor is removed as the avatar of the Ability System - Should be overridden by the child classes. */
	virtual void OnAbilitySystemUninitialized();

	/** Begins the death sequence for the character (disables collision, disables movement, etc...) */
	virtual void OnDeathStarted(AActor* InOwningActor);

	/** Ends the death sequence for the character (detaches controller, destroys pawn, etc...) */
	virtual void OnDeathFinished(AActor* InOwningActor);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName="On Death Finished"), Category = "Obsidian|Character")
	void BP_OnDeathFinished();

	void DestroyDueToDeath();
	void UninitAndDestroy();

	/** Removes the dead Character from the World, this is the very last step of the death sequence. */
	virtual void DestroyDeadCharacter();

	UPROPERTY(VisibleAnywhere, Category = "Obsidian|Character")
	TObjectPtr<UObsidianPawnExtensionComponent> PawnExtComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComp;

	UPROPERTY(VisibleAnywhere, Category = "Obsidian|Character")
	TObjectPtr<UMassAgentComponent> MassAgentComponent;

	/**
	 * Sockets used mostly for combat reasons, spawning projectiles.
	 */

	UPROPERTY(EditAnywhere, Category = "Obsidian|Combat|Setup")
	FName WeaponSocketName = FName();

	UPROPERTY(EditAnywhere, Category = "Obsidian|Combat|Setup")
	FName RightHandSocketName = FName();

	UPROPERTY(EditAnywhere, Category = "Obsidian|Combat|Setup")
	FName LeftHandSocketName = FName();

	/** Offset for how much the ability spawn location is moved from the player character. */ 
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Combat")
	float DefaultAbilitySocketLocationOffset = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Combat")
	bool bCanHitReact = false;

	/**
	 * Character Properties
	 */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|CharacterProperties|Death")
	TArray<TObjectPtr<UAnimMontage>> DeathMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Obsidian|CharacterProperties")
	TObjectPtr<UNiagaraSystem> BloodEffect;

	/** DEPRECATED */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Death|Ragdoll", meta=(DeprecatedProperty))
	FName RagdollImpulseBone = FName();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Death|Ragdoll", meta=(DeprecatedProperty))
	float RagdollImpulseStrength = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Death|Ragdoll", meta=(DeprecatedProperty))
	bool bRagdollWithImpulse = true;
};

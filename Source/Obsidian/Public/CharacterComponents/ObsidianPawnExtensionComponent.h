// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>


#include <Components/PawnComponent.h>
#include "ObsidianPawnExtensionComponent.generated.h"

class UObsidianAbilitySystemComponent;
class UObsidianPawnData;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianPawnExtensionComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	UObsidianPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = "Obsidian|Pawn")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const
	{
		return AbilitySystemComponent;
	}

	/** Returns the hero component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Obsidian|ExtComp")
	static UObsidianPawnExtensionComponent* FindPawnExtComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UObsidianPawnExtensionComponent>() : nullptr);
	}
	
	const UObsidianPawnData* GetPawnData() const
	{
		return PawnData;
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Should be called by the owning Pawn to become the Ability System's avatar actor. */
	void InitializeAbilitySystem(UObsidianAbilitySystemComponent* InASC, AActor* InOwnerActor);

	/** Should be called by the owning Pawn to remove itself as the Ability System's avatar actor. */
	void UninitializeAbilitySystem();

	/** Register with the OnAbilitySystemInitialized delegate and broadcast if our Pawn has been registered with the Ability System Component */
	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
	
	/** Register with the OnAbilitySystemInitialized delegate, this is fired when our pawn is removed as the Ability System's avatar actor */
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Obsidian|Pawn")
	TObjectPtr<const UObsidianPawnData> PawnData;

	UPROPERTY()
	TObjectPtr<UObsidianAbilitySystemComponent> AbilitySystemComponent;

	/** Delegate fired when our Pawn becomes the Ability System's avatar actor. */
	FSimpleMulticastDelegate OnAbilitySystemInitialized;

	/** Delegate fired when our Pawn is removed as the Ability System's avatar actor. */
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
};

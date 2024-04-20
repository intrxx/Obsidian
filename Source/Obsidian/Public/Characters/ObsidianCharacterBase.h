// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "AbilitySystemInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ObsidianCharacterBase.generated.h"

class UObsidianAbilitySystemComponent;
class UObsidianPawnExtensionComponent;

UCLASS(Abstract)
class OBSIDIAN_API AObsidianCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AObsidianCharacterBase();

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Character")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

protected:
	/** Fired when this actor is initialized as the avatar of the Ability System - Should be overridden by the child classes */
	virtual void OnAbilitySystemInitialized();

	/** Fired when this actor is removed as the avatar of the Ability System - Should be overridden by the child classes */
	virtual void OnAbilitySystemUninitialized();
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian|Character")
	TObjectPtr<UObsidianPawnExtensionComponent> PawnExtComp;
	
	UPROPERTY(EditAnywhere, Category = "Obsidian|Combat")
	TObjectPtr<USkeletalMeshComponent> RightHandEquipmentMesh;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Combat")
	TObjectPtr<USkeletalMeshComponent> LeftHandEquipmentMesh;
};

// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Obsidian|Pawn")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const {return AbilitySystemComponent;}

	/** Returns the hero component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Obsidian|ExtComp")
	static UObsidianPawnExtensionComponent* FindPawnExtComponent(const AActor* Actor) {return (Actor ? Actor->FindComponentByClass<UObsidianPawnExtensionComponent>() : nullptr);}
	
	const UObsidianPawnData* GetPawnData() const {return PawnData;}
	
protected:
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Obsidian|Pawn")
	TObjectPtr<const UObsidianPawnData> PawnData;

	UPROPERTY()
	TObjectPtr<UObsidianAbilitySystemComponent> AbilitySystemComponent;
};

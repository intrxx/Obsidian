// Copyright 2024 Michał Ogiński

#pragma once

#include "AbilitySystemInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ObsidianPlayerState.generated.h"

class UObsidianCommonAttributeSet;
class UObsidianHeroAttributeSet;
class UObsidianAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AObsidianPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerState")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UObsidianAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UObsidianHeroAttributeSet> HeroAttributeSet;
	UPROPERTY()
	TObjectPtr<UObsidianCommonAttributeSet> CommonAttributeSet;
};

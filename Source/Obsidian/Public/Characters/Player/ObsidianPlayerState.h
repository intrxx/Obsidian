// Copyright 2024 Michał Ogiński

#pragma once

#include "AbilitySystemInterface.h"
#include "CoreMinimal.h"
#include "ModularPlayerState.h"

#include "ObsidianPlayerState.generated.h"

class AObsidianHero;
class AObsidianPlayerController;
class UObsidianCommonAttributeSet;
class UObsidianHeroAttributeSet;
class UObsidianAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHeroLevelUp, const uint8, NewLevel);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianPlayerState : public AModularPlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AObsidianPlayerState(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerState")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const { return AbilitySystemComponent; }

	TObjectPtr<UObsidianHeroAttributeSet> GetObsidianAttributeSet() const { return HeroAttributeSet; }

	AObsidianPlayerController* GetObsidianPlayerController() const;
	AObsidianHero* GetObsidianHero() const;

	uint8 GetHeroLevel() const { return HeroLevel; }

	FString GetObsidianPlayerName() { return PlayerName; }

	void IncreaseHeroLevel();

	// Loading purposes, don't really like it like this
	void SetHeroLevel(const uint8 InLevel) { HeroLevel = InLevel; }
	void SetObsidianPlayerName(const FString& InName) { PlayerName = InName; }

	//~ Begin IAbilitySystemInterface Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface Interface

	//~ Begin UObject Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface

	UPROPERTY(BlueprintAssignable, Category = "Obsidian|PlayerState")
	FOnHeroLevelUp OnHeroLevelUp;

private:
	UFUNCTION()
	void OnRep_HeroLevel();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UObsidianAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UObsidianHeroAttributeSet> HeroAttributeSet;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_HeroLevel)
	uint8 HeroLevel = 1;

	UPROPERTY(VisibleAnywhere)
	FString PlayerName = FString();
};

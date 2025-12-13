// Copyright 2024 Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"

// ~ Project


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
	AObsidianPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerState")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const
	{
		return AbilitySystemComponent;
	}

	TObjectPtr<UObsidianHeroAttributeSet> GetObsidianAttributeSet() const
	{
		return HeroAttributeSet;
	}

	AObsidianPlayerController* GetObsidianPlayerController() const;
	AObsidianHero* GetObsidianHero() const;
	
	uint8 GetHeroLevel() const
	{
		return HeroLevel;
	}

	FString GetObsidianPlayerName();

	void IncreaseHeroLevel();

	// Loading purposes, don't really like it like this
	void SetHeroLevel(const uint8 InLevel);
	void SetObsidianPlayerName(const FString& InName);

public:
	UPROPERTY(BlueprintAssignable, Category = "Obsidian|PlayerState")
	FOnHeroLevelUp OnHeroLevelUp;
	
private:
	UFUNCTION()
	void OnRep_HeroLevel();
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UObsidianAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UObsidianHeroAttributeSet> HeroAttributeSet;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_HeroLevel)
	uint8 HeroLevel = 1;

	UPROPERTY(VisibleAnywhere)
	FString PlayerName = FString();
};

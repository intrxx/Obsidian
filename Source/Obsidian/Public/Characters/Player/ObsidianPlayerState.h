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
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerState")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const
	{
		return AbilitySystemComponent;
	}
	
	int32 GetHeroLevel() const
	{
		return HeroLevel;
	}

	void IncreaseHeroLevel();

private:
	UFUNCTION()
	void OnRep_HeroLevel();
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UObsidianAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UObsidianHeroAttributeSet> HeroAttributeSet;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_HeroLevel)
	int32 HeroLevel = 1;

};

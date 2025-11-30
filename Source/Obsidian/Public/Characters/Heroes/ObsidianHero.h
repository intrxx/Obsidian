// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include "ObsidianTypes/ObsidianCoreTypes.h"

#include "Characters/ObsidianCharacterBase.h"
#include "Game/Save/ObsidianSaveableInterface.h"
#include "ObsidianHero.generated.h"

struct FObsidianGenericAttributes;

class AObsidianDroppableItem;
class UObsidianHeroHealthBar_Simple;
class UObsidianHeroHealthBar;
class UObsidianWidgetBase;
class UWidgetComponent;
class AObsidianPlayerController;
class UObsidianHeroAttributesComponent;
class AObsidianPlayerState;
class USpringArmComponent;
class UCameraComponent;
class UObsidianPlayerInputManager;

/**
 * Main class for Hero characters in Obsidian.
 */
UCLASS()
class OBSIDIAN_API AObsidianHero : public AObsidianCharacterBase, public IObsidianSaveableInterface
{
	GENERATED_BODY()
public:
	AObsidianHero(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Hero")
	AObsidianPlayerState* GetObsidianPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Hero")
	AObsidianPlayerController* GetObsidianPlayerController() const;

	UObsidianWidgetBase* GetHealthBarWidget() const;

	UObsidianPlayerInputManager* GetPlayerInputManager() const;

	EObsidianHeroClass GetHeroClass() const
	{
		return HeroClass;
	}
	
	//~ Start of CombatInterface
	virtual uint8 GetCharacterLevel() override;
	virtual bool IsDeadOrDying_Implementation() const override;
	virtual AActor* GetAvatarActor_Implementation() override;
	virtual FVector GetAbilitySocketLocationFromLHWeapon_Implementation() override;
	virtual FVector GetAbilitySocketLocationFromRHWeapon_Implementation() override;
	//~ End of CombatInterface

	//~ Start of SaveableInterface
	virtual void SaveData(UObsidianHeroSaveGame* SaveObject) override;
	virtual void LoadData(UObsidianHeroSaveGame* SaveObject) override;
	//~ End of SaveableInterface

	/** Updates when boss sees Player, BossActor will be nullptr when Boss lost sight of Player, this is by design and might change. */
	UFUNCTION(Client, Reliable)
	void ClientUpdateBossDetectingPlayer(AActor* BossActor, const bool bSeenPlayer);

	void IncreaseHeroLevel() const;
	uint8 GetHeroLevel() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	//~ Start of AObsidianCharacterBase
	virtual void OnAbilitySystemInitialized() override;
	virtual void OnAbilitySystemUninitialized() override;
	
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor) override;

	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor) override;
	//~ End of AObsidianCharacterBase

	void InitializeUI(UObsidianAbilitySystemComponent* ObsidianASC) const;

	void FillGenericAttribures(FObsidianGenericAttributes& GenericAttributes);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Hero")
	EObsidianHeroClass HeroClass = EObsidianHeroClass::None;
	
private:
	void InitializeHealthBar() const;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Hero", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianPlayerInputManager> PlayerInputManager;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Hero", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianHeroAttributesComponent> HeroAttributesComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Hero", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Hero", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Hero", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> HealthBarWidgetComp;

	//@Note, this health bars does not work for the simulated clients if we are the server,
	// I'm okay with that but that should be something to consider implementing
	
	/** Health bar to set on locally controller Player. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|HealthBar", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianHeroHealthBar> AutonomousHealthBarClass;

	/** Health bar to set on simulated proxy Player. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|HealthBar", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianHeroHealthBar_Simple> SimulatedHealthBarClass;
};

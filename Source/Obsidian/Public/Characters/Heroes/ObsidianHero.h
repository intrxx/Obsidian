// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"

#include "Characters/ObsidianCharacterBase.h"
#include "Game/Save/ObsidianSaveableInterface.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"

#include "ObsidianHero.generated.h"

struct FObsidianGenericAttributes;

class UGameplayCameraComponent;
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
class OBSIDIAN_API AObsidianHero : public AObsidianCharacterBase, public IObsidianSaveableInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	AObsidianHero(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	
	// ~ Start of ACharacter interface
	virtual void SetupPlayerInputComponent(UInputComponent* InPlayerInputComponent) override;
	virtual void PossessedBy(AController* InNewController) override;
	virtual void OnRep_PlayerState() override;
	// ~ Start of ACharacter interface
	
	// ~ Start of IGenericTeamAgentInterface interface
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	// ~ End of IGenericTeamAgentInterface interface
	
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
	virtual void SaveData(UObsidianHeroSaveGame* InSaveObject) override;
	virtual void LoadData(UObsidianHeroSaveGame* InSaveObject) override;
	//~ End of SaveableInterface

	/** Updates when boss sees Player, BossActor will be nullptr when Boss lost sight of Player, this is by design and might change. */
	UFUNCTION(Client, Reliable)
	void ClientUpdateBossDetectingPlayer(AActor* InBossActor, const bool bInSeenPlayer);

	void IncreaseHeroLevel() const;
	uint8 GetHeroLevel() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type InEndPlayReason) override;
	
	//~ Start of AObsidianCharacterBase
	virtual void OnAbilitySystemInitialized() override;
	virtual void OnAbilitySystemUninitialized() override;
	
	UFUNCTION()
	virtual void OnDeathStarted(AActor* InOwningActor) override;

	UFUNCTION()
	virtual void OnDeathFinished(AActor* InOwningActor) override;
	//~ End of AObsidianCharacterBase

	void InitializeUI(UObsidianAbilitySystemComponent* InObsidianASC) const;

	void FillGenericAttribures(FObsidianGenericAttributes& OutGenericAttributes);

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
	TObjectPtr<UGameplayCameraComponent> GameplayCameraComponent;

	//TODO(intrxx) Delete after GameplayCamera is stable
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Hero", meta = (AllowPrivateAccess = "true"))
	// TObjectPtr<UCameraComponent> CameraComponent;
	//
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Hero", meta = (AllowPrivateAccess = "true"))
	// TObjectPtr<USpringArmComponent> SpringArmComponent;
	// ~ End of Delete after GameplayCamera is stable
	
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
	
	FGenericTeamId TeamId;
};

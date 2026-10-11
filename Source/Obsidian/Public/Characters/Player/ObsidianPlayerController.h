// Copyright 2024 Michał Ogiński

#pragma once

#include "CommonPlayerController.h"
#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"

#include "ObsidianTypes/ObsidianCoreTypes.h"

#include "ObsidianPlayerController.generated.h"

class UObsidianItemManagerComponent;
class UObsidianCraftingComponent;
class UObsidianSharedStashSaveGame;
class AObsidianHero;
class UObsidianStash;
class UObsidianEquipmentComponent;
struct FObsidianDamageTextProps;
class AObsidianCharacterBase;
class UObsidianDamageNumberWidgetComp;
class AObsidianHUD;
class UObsidianAbilitySystemComponent;
class AObsidianPlayerState;
class UObsidianInventoryComponent;
class AObsidianItemSpawner;
class UObsidianPlayerStashComponent;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnEnemyActorHovered, AActor*, TargetActor, const bool, bHoveredOver);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnBossDetectedPlayer, AActor*, BossActor, const bool, bSeen);

/**
 * Base class for Obsidian player characters player controller.
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianPlayerController : public ACommonPlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AObsidianPlayerController(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerController")
	UObsidianInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerController")
	UObsidianEquipmentComponent* GetEquipmentComponent() const { return EquipmentComponent; }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerController")
	UObsidianPlayerStashComponent* GetPlayerStashComponent() const { return PlayerStashComponent; }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerController")
	UObsidianCraftingComponent* GetCraftingComponent() const { return CraftingComponent; }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerController")
	UObsidianItemManagerComponent* GetItemManagerComponent() const { return ItemManagerComponent; }

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerController")
	EObsidianHeroClass GetHeroClass() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerController")
	AObsidianPlayerState* GetObsidianPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerController")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerController")
	AObsidianHUD* GetObsidianHUD() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerController")
	AObsidianHero* GetObsidianHero() const;

	void TogglePlayerStash(const bool bInShowStash) const;

	UFUNCTION(Client, Reliable)
	void ClientShowDamageNumber(const FObsidianDamageTextProps& InDamageTextProps, AObsidianCharacterBase* InTargetCharacter);

	/** Updates when Player hovers over Regular Enemy Target, TargetActor will be nullptr after removing the mouse from target, this is by design and might change. */
	void UpdateHoveredRegularEnemyTarget(AActor* InTargetActor, const bool bInHoveredOver) const;

	UFUNCTION(Server, Reliable)
	void ServerSpawnItemFromSpawner(AObsidianItemSpawner* InItemSpawner);

	//~ Begin APlayerController Interface
	virtual void PostProcessInput(const float InDeltaTime, const bool bInGamePaused) override;

	virtual void ReceivedPlayer() override;
	//~ End APlayerController Interface

	//~ Begin AActor Interface
	virtual void PostInitializeComponents() override;
	//~ End AActor Interface

	//~ Begin IGenericTeamAgentInterface Interface
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	//~ End IGenericTeamAgentInterface Interface

	FOnEnemyActorHovered OnEnemyActorHoveredDelegate;
	FOnBossDetectedPlayer OnBossDetectedPlayerDelegate;

protected:
	void LoadSharedStashData(UObsidianSharedStashSaveGame* InSharedStashSaveGame);

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Cursor")
	TObjectPtr<UUserWidget> DefaultCursor;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|DamageNumber")
	TSubclassOf<UObsidianDamageNumberWidgetComp> DamageNumberWidgetCompClass;

	UPROPERTY(VisibleAnywhere, Category = "Obsidian|InventoryItems")
	TObjectPtr<UObsidianInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, Category = "Obsidian|InventoryItems")
	TObjectPtr<UObsidianEquipmentComponent> EquipmentComponent;

	UPROPERTY(VisibleAnywhere, Category = "Obsidian|InventoryItems")
	TObjectPtr<UObsidianPlayerStashComponent> PlayerStashComponent;

	UPROPERTY(VisibleAnywhere, Category = "Obsidian|InventoryItems")
	TObjectPtr<UObsidianCraftingComponent> CraftingComponent;

	UPROPERTY(VisibleAnywhere, Category = "Obsidian|InventoryItems")
	TObjectPtr<UObsidianItemManagerComponent> ItemManagerComponent;

	FDelegateHandle SharedStashDataLoadDelegateHandle;
	bool bRequestedSharedStashData = false;

	FGenericTeamId TeamId;
};

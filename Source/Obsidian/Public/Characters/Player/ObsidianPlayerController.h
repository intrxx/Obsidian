// Copyright 2024 Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include "ObsidianTypes/ObsidianCoreTypes.h"

#include "CommonPlayerController.h"
#include "ObsidianPlayerController.generated.h"

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
UCLASS()
class OBSIDIAN_API AObsidianPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()
public:
	AObsidianPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostInitializeComponents() override;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerController")
	UObsidianInventoryComponent* GetInventoryComponent() const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerController")
	UObsidianEquipmentComponent* GetEquipmentComponent() const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerController")
	UObsidianPlayerStashComponent* GetPlayerStashComponent() const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerController")
	UObsidianCraftingComponent* GetCraftingComponent() const;

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

	void TogglePlayerStash(const bool bShowStash) const;
	
	UFUNCTION(Client, Reliable)
	void ClientShowDamageNumber(const FObsidianDamageTextProps& DamageTextProps, AObsidianCharacterBase* TargetCharacter);

	/** Updates when Player hovers over Regular Enemy Target, TargetActor will be nullptr after removing the mouse from target, this is by design and might change. */
	void UpdateHoveredRegularEnemyTarget(AActor* TargetActor, const bool bHoveredOver) const;

	UFUNCTION(Server, Reliable)
	void ServerSpawnItemFromSpawner(AObsidianItemSpawner* ItemSpawner);

public:
	FOnEnemyActorHovered OnEnemyActorHoveredDelegate;
	FOnBossDetectedPlayer OnBossDetectedPlayerDelegate;
	
protected:
	virtual void BeginPlay() override;

	void LoadSharedStashData(UObsidianSharedStashSaveGame* SharedStashSaveGame);

protected:
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

	FDelegateHandle SharedStashDataLoadDelegateHandle;
};

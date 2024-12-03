// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ObsidianPlayerController.generated.h"

struct FObsidianDamageTextProps;
class AObsidianCharacterBase;
class UObsidianDamageNumberWidgetComp;
class AObsidianHUD;
class UObsidianAbilitySystemComponent;
class AObsidianPlayerState;
class UObsidianInventoryComponent;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnEnemyActorHovered, AActor*, TargetActor, const bool, bHoveredOver);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnBossDetectedPlayer, AActor*, BossActor, const bool, bSeen);

/**
 * Base class for Obsidian player characters player controller.
 */
UCLASS()
class OBSIDIAN_API AObsidianPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AObsidianPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerController")
	UObsidianInventoryComponent* GetInventoryComponent() const
	{
		return InventoryComponent;
	}

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerController")
	AObsidianPlayerState* GetObsidianPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerController")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerController")
	AObsidianHUD* GetObsidianHUD() const;
	
	UFUNCTION(Client, Reliable)
	void ClientShowDamageNumber(const FObsidianDamageTextProps& DamageTextProps, AObsidianCharacterBase* TargetCharacter);

	/** Updates when Player hovers over Regular Enemy Target, TargetActor will be nullptr after removing the mouse from target, this is by design and might change. */
	void UpdateHoveredRegularEnemyTarget(AActor* TargetActor, const bool bHoveredOver) const;

public:
	FOnEnemyActorHovered OnEnemyActorHoveredDelegate;
	FOnBossDetectedPlayer OnBossDetectedPlayerDelegate;
	
protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Cursor")
	TObjectPtr<UUserWidget> DefaultCursor;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|DamageNumber")
	TSubclassOf<UObsidianDamageNumberWidgetComp> DamageNumberWidgetCompClass;

	UPROPERTY(VisibleAnywhere, Category = "Obsidian|Character")
	TObjectPtr<UObsidianInventoryComponent> InventoryComponent;
	
};

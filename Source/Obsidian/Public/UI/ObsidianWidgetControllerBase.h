 // Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "Characters/Player/ObsidianLocalPlayer.h"
#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"

#include "ObsidianWidgetControllerBase.generated.h"

class UObsidianLocalPlayer;
class UObsidianPlayerStashComponent;
class UObsidianCraftingComponent;
class AObsidianPlayerController;
class AObsidianPlayerState;
class UObsidianEquipmentComponent;
class UObsidianInventoryComponent;
class UObsidianHeroAttributesComponent;
class UObsidianAbilitySystemComponent;
class UObsidianAttributesComponent;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FObsidianWidgetControllerParams
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AObsidianPlayerController> ObsidianPlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AObsidianPlayerState> ObsidianPlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UObsidianLocalPlayer> ObsidianLocalPlayer = nullptr;
};

/**
 * 
 */
UCLASS(Abstract)
class OBSIDIAN_API UObsidianHeroWidgetControllerBase : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|WidgetController")
	void SetWidgetControllerParams(const FObsidianWidgetControllerParams& WidgetControllerParams);
	
	/**
	 * This function is called when the initial setup for Widget Controller is completed,
	 * widget controller contains valid data.
     */
	virtual void OnWidgetControllerSetupCompleted();

	AObsidianPlayerController* GetOwningPlayerController() const
	{
		return OwnerPlayerController.Get();
	}

	AObsidianPlayerState* GetOwningPlayerState() const
	{
		return OwnerPlayerState.Get();
	}

protected:
	virtual void HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|HeroWidgetController")
	TWeakObjectPtr<AObsidianPlayerController> OwnerPlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|HeroWidgetController")
	TWeakObjectPtr<UObsidianLocalPlayer> OwnerLocalPlayer;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|HeroWidgetController")
	TWeakObjectPtr<AObsidianPlayerState> OwnerPlayerState;
};


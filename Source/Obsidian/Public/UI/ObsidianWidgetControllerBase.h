 // Copyright 2024 out of sCope team - Michał Ogiński

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
	TObjectPtr<UObsidianAbilitySystemComponent> ObsidianAbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UObsidianHeroAttributesComponent> AttributesComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UObsidianInventoryComponent> InventoryComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UObsidianEquipmentComponent> EquipmentComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UObsidianPlayerStashComponent> PlayerStashComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UObsidianLocalPlayer> ObsidianLocalPlayer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UObsidianCraftingComponent> CraftingComponent = nullptr;
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
		return ObsidianPlayerController;
	}

	AObsidianPlayerState* GetOwningPlayerState() const
	{
		return ObsidianPlayerState;
	}

protected:
	virtual void HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC);

protected:
	//TODO(intrxx) replace TObjectPtr with TWeakObjectPtr for these pointers
	
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|HeroWidgetController")
	TObjectPtr<AObsidianPlayerController> ObsidianPlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|HeroWidgetController")
	TObjectPtr<UObsidianLocalPlayer> ObsidianLocalPlayer;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|HeroWidgetController")
	TObjectPtr<AObsidianPlayerState> ObsidianPlayerState;
	
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|HeroWidgetController")
	TObjectPtr<UObsidianAbilitySystemComponent> ObsidianAbilitySystemComponent;
	
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HeroWidgetController")
	TObjectPtr<UObsidianHeroAttributesComponent> AttributesComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HeroWidgetController")
	TObjectPtr<UObsidianInventoryComponent> InventoryComponent;
	
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HeroWidgetController")
	TObjectPtr<UObsidianEquipmentComponent> EquipmentComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HeroWidgetController")
	TObjectPtr<UObsidianPlayerStashComponent> PlayerStashComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|HeroWidgetController")
	TWeakObjectPtr<UObsidianCraftingComponent> OwnerCraftingComponent;
};


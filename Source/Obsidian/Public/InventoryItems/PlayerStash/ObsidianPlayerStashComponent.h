// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "InventoryItems/PlayerStash/ObsidianStashItemList.h"

#include "Components/ActorComponent.h"
#include "ObsidianPlayerStashComponent.generated.h"

class UObsidianStashTabsConfig;
class UObsidianInventoryItemDefinition;
class UObsidianInventoryItemInstance;

/**
 * Primary Player Stash Component of Obsidian to be used by Players.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBSIDIAN_API UObsidianPlayerStashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObsidianPlayerStashComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/** Returns the Player Stash Component if one exists on the specified actor, will be nullptr otherwise */
	UFUNCTION(BlueprintPure, Category = "Obsidian|Inventory")
	static UObsidianPlayerStashComponent* FindPlayerStashComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UObsidianPlayerStashComponent>() : nullptr);
	}

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UObsidianStashTabsConfig* GetStashTabConfig() const;

	/** Finds all stacks in the inventory for given item type with item Def. */
	int32 FindAllStacksForGivenItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef);
	
	/** Finds all stacks in the inventory for gixven item type with item Instance. */
	int32 FindAllStacksForGivenItem(const UObsidianInventoryItemInstance* ItemInstance);

protected:
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TObjectPtr<UObsidianStashTabsConfig> StashTabsConfig;

private:
	/**
	 * Actual array of stashed items which is FFastArraySerializer.
	 */
	UPROPERTY(Replicated)
	FObsidianStashItemList StashItemList;
};

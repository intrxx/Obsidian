// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianInventoryGrid.h"
#include "Components/ActorComponent.h"
#include "ObsidianInventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedToInventorySignature, UObsidianInventoryItemInstance* ItemInstance);

/**
 * Primary Inventory Component of Obsidian to be used by Characters.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBSIDIAN_API UObsidianInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObsidianInventoryComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Returns the Inventory Component if one exists on the specified actor, will be nullptr otherwise */
	UFUNCTION(BlueprintPure, Category = "Obsidian|Inventory")
	static UObsidianInventoryComponent* FindInventoryComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UObsidianInventoryComponent>() : nullptr);
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	int32 GetTotalItemCountByDefinition(TSubclassOf<UObsidianInventoryItemDefinition> ItemDef) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Obsidian|Inventory")
	TArray<UObsidianInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|Inventory")
	UObsidianInventoryItemInstance* FindFirstItemStackForDefinition(TSubclassOf<UObsidianInventoryItemDefinition> ItemDef) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool CanAddItemDefinition(FVector2D& OutAvailablePosition, TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	UObsidianInventoryItemInstance* AddItemDefinition(TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void RemoveItemInstance(UObsidianInventoryItemInstance* InstanceToRemove);

	bool ConsumeItemsByDefinition(TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 NumberOfItemsToConsume);
	
	//~ Start of UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~ End of UObject interface

public:
	FOnItemAddedToInventorySignature OnItemAddedToInventoryDelegate;

private:
	void InitInventoryState();
	
	void Item_MarkSpace(const FVector2D AtPosition, const UObsidianInventoryItemInstance* ItemInstance);
	
private:
	UPROPERTY(Replicated)
	FObsidianInventoryGrid InventoryGrid;

	UPROPERTY()
	TMap<FVector2D, bool> InventoryStateMap;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|InventorySetup")
	int32 InventoryGridWidth = 12;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|InventorySetup")
	int32 InventoryGridHeight = 5;
	
	int32 InventoryGridSize = 0;
};

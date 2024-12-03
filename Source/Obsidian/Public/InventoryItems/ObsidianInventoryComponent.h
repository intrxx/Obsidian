// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianInventoryGrid.h"
#include "Components/ActorComponent.h"
#include "ObsidianInventoryComponent.generated.h"

class UObsidianInventoryWidgetController;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemAddedToInventorySignature, UObsidianInventoryItemInstance* ItemInstance,
                                     FVector2D DesiredPosition);

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
	
	int32 GetTotalItemCountByDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Obsidian|Inventory")
	TArray<UObsidianInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|Inventory")
	UObsidianInventoryItemInstance* FindFirstItemStackForDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool CanAddItemDefinition(FVector2D& OutAvailablePosition, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, int32 StackCount = 1);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool CanAddItemDefinitionAtSpecifiedSlot(const FVector2D& SpecifiedSlot, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	UObsidianInventoryItemInstance* AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	UObsidianInventoryItemInstance* AddItemDefinitionToSpecifiedSlot(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FVector2D& ToSlot, const int32 StackCount = 1);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool CanAddItemInstance(FVector2D& OutAvailablePosition, UObsidianInventoryItemInstance* Instance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void RemoveItemInstance(UObsidianInventoryItemInstance* InstanceToRemove);

	bool ConsumeItemsByDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 NumberOfItemsToConsume);
	
	//~ Start of UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~ End of UObject interface

public:
	FOnItemAddedToInventorySignature OnItemAddedToInventoryDelegate;

private:
	FVector2D GetItemLocationFromGrid(UObsidianInventoryItemInstance* ItemInstance) const;
	
	void InitInventoryState();
	
	void Item_MarkSpace(const FVector2D AtPosition, const UObsidianInventoryItemInstance* ItemInstance);
	void Item_UnMarkSpace(const FVector2D AtPosition, const UObsidianInventoryItemInstance* ItemInstance);

	/** Checks if the item  */
	bool CheckAvailablePosition(const TArray<FVector2D>& ItemGridSize, FVector2D& OutAvailablePosition);
	bool CheckSpecifiedPosition(const TArray<FVector2D>& ItemGridSize, const FVector2D& SpecifiedPosition);

	/** Internal usage only, this returns the internal Location To Instance Map. */
	TMap<FVector2D, UObsidianInventoryItemInstance*> Internal_GetLocationToInstanceMap();
	
private:
	friend UObsidianInventoryWidgetController;
	
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

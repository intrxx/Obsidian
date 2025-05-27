// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UObject/Interface.h"
#include "ObsidianPickableInterface.generated.h"

class UObsidianInventoryComponent;
class UObsidianInventoryItemDefinition;
class UObsidianInventoryItemInstance;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPickupTemplate
{
	GENERATED_BODY()

public:
	FPickupTemplate(){};
	FPickupTemplate(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const int32 InStackCount)
		: ItemDef(InItemDef)
		, StackCount(InStackCount)
	{};

	bool IsValid() const;
	
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = nullptr;

	UPROPERTY(EditAnywhere)
	int32 StackCount = 1;
};

USTRUCT(BlueprintType)
struct FPickupInstance
{
	GENERATED_BODY()
	
public:
	FPickupInstance(){};
	FPickupInstance(UObsidianInventoryItemInstance* InItemInstance)
		: Item(InItemInstance)
	{};

	bool IsValid() const;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UObsidianInventoryItemInstance> Item = nullptr;
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPickupContent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPickupInstance Instance = FPickupInstance(nullptr);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPickupTemplate Template = FPickupTemplate(nullptr, 1);
};

/**
 * 
 */
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UObsidianPickableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface that adds functionality to item to be picked.
 */
class OBSIDIAN_API IObsidianPickableInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Pickable")
	virtual FPickupContent GetPickupContent() const = 0;

	// This is a direct way to add items to pickup, I'm okay with it for now but idk if it's the way to go
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Pickable")
	virtual void AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd) = 0;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Pickable")
	virtual void AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 ItemStacks) = 0;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Pickable")
	virtual void OverrideTemplateStacks(const int32 TemplateIndex) = 0;
	
	//
};

/**
 * Pickable helper functions.
 */
UCLASS()
class UObsidianPickableStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UObsidianPickableStatics(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintPure)
	static TScriptInterface<IObsidianPickableInterface> GetPickableFromActor(AActor* Actor);

	// Not really using it as adding item to inventory is more complicated due to stacks manipulation
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "Ability"))
	static void AddPickupToInventory(UObsidianInventoryComponent* InventoryComponent, const TScriptInterface<IObsidianPickableInterface> Pickup);
};

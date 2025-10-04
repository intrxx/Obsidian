// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianItemAffixStack.h"

#include "UObject/Interface.h"
#include "ObsidianPickableInterface.generated.h"

class UObsidianInventoryComponent;
class UObsidianInventoryItemDefinition;
class UObsidianInventoryItemInstance;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianPickupTemplate
{
	GENERATED_BODY()

public:
	FObsidianPickupTemplate(){};
	FObsidianPickupTemplate(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const FObsidianItemGeneratedData& InGeneratedData)
		: ItemDef(InItemDef)
		, ItemGeneratedData(InGeneratedData)
	{};

	bool IsValid() const;
	
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = nullptr;

	UPROPERTY()
	FObsidianItemGeneratedData ItemGeneratedData;
};

USTRUCT(BlueprintType)
struct FObsidianPickupInstance
{
	GENERATED_BODY()
	
public:
	FObsidianPickupInstance(){};
	FObsidianPickupInstance(UObsidianInventoryItemInstance* InItemInstance)
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
struct FObsidianPickupContent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FObsidianPickupInstance Instance = FObsidianPickupInstance(nullptr);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FObsidianPickupTemplate Template = FObsidianPickupTemplate(nullptr, 1);
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
	virtual FObsidianPickupContent GetPickupContent() const = 0;

	// This is a direct way to add items to pickup, I'm okay with it for now but idk if it's the way to go
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Pickable")
	virtual void AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd) = 0;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Pickable")
	virtual void AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FObsidianItemGeneratedData& InGeneratedData) = 0;
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
};

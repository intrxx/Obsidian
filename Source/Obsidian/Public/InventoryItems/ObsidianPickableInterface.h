// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ObsidianPickableInterface.generated.h"

class UObsidianInventoryComponent;
class UObsidianInventoryItemDefinition;
class UObsidianInventoryItemInstance;

USTRUCT(BlueprintType)
struct FPickupTemplate
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int32 StackCount = 1;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef;
};

USTRUCT(BlueprintType)
struct FPickupInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UObsidianInventoryItemInstance> Item = nullptr;
};

USTRUCT(BlueprintType)
struct FPickupContent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPickupInstance> Instances;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPickupTemplate> Templates;
};

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
	static TScriptInterface<IObsidianPickableInterface> GetFirstPickableFromActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "Ability"))
	static void AddPickupToInventory(UObsidianInventoryComponent* InventoryComponent, const TScriptInterface<IObsidianPickableInterface> Pickup);
};

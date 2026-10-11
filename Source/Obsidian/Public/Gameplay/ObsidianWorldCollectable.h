// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "InventoryItems/ObsidianPickableInterface.h"

#include "ObsidianWorldCollectable.generated.h"

UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianWorldCollectable : public AActor, public IObsidianPickableInterface
{
	GENERATED_BODY()

public:
	AObsidianWorldCollectable(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	FObsidianPickupInstance GetPickupInstanceFromPickupContent() const;
	FObsidianPickupTemplate GetPickupTemplateFromPickupContent() const;

	//~ Begin UObject Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface

	//~ Begin IObsidianPickableInterface Interface
	virtual FObsidianPickupContent GetPickupContent() const override { return PickupContent; }
	virtual void AddItemInstance(UObsidianInventoryItemInstance* InInstanceToAdd) override;
	virtual void AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> InItemDef,
		const FObsidianItemGeneratedData& InGeneratedData) override;
	virtual void OverrideTemplateStacks(const int32 InNewItemStacks) override;
	//~ End IObsidianPickableInterface Interface

	//~ Begin AActor Interface
	virtual bool ReplicateSubobjects(UActorChannel* InChannel, FOutBunch* InBunch, FReplicationFlags* InRepFlags) override;
	//~ End AActor Interface

protected:
	UFUNCTION()
	virtual void OnRep_PickupContent();

	bool CarriesItemInstance() const;
	bool CarriesItemDef() const;
	bool CarriesBoth() const;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_PickupContent, Category = "Obsidian")
	FObsidianPickupContent PickupContent;
};

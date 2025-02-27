// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems/ObsidianPickableInterface.h"
#include "GameFramework/Actor.h"
#include "ObsidianWorldCollectable.generated.h"

UCLASS()
class OBSIDIAN_API AObsidianWorldCollectable : public AActor, public IObsidianPickableInterface
{
	GENERATED_BODY()
	
public:	
	AObsidianWorldCollectable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FPickupContent GetPickupContent() const override;
	FPickupInstance GetPickupInstanceFromPickupContent() const;
	FPickupTemplate GetPickupTemplateFromPickupContent() const;
	
	virtual void AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd) override;
	virtual void AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 ItemStacks) override;
	virtual void OverrideTemplateStacks(const int32 NewItemStacks) override;

	//~ Start of UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~ End of UObject interface

protected:
	UFUNCTION()
	virtual void OnRep_PickupContent();
	
	bool CarriesItemInstance() const;
	bool CarriesItemDef() const;
	bool CarriesBoth() const;
	
protected:
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_PickupContent, Category = "Obsidian")
	FPickupContent PickupContent;

};

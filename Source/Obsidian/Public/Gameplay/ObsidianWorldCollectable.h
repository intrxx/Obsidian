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

	virtual FPickupContent GetPickupContent() const override;
	FPickupInstance GetFirstPickupInstanceFromPickupContent() const;
	FPickupTemplate GetFirstPickupTemplateFromPickupContent() const;
	
	virtual void AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd) override;
	virtual void AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 ItemStacks) override;
	virtual void OverrideTemplateStacks(const int32 NewItemStacks, const int32 TemplateIndex = 0) override;

protected:
	bool CarriesItemInstance() const;
	bool CarriesItemDef() const;
	bool CarriesBoth() const;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Obsidian")
	FPickupContent StaticContent;
};

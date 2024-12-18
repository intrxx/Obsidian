// Copyright 2024 out of sCope team - Michał Ogiński


#include "Gameplay//ObsidianWorldCollectable.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"

AObsidianWorldCollectable::AObsidianWorldCollectable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

FPickupContent AObsidianWorldCollectable::GetPickupContent() const
{
	return StaticContent;
}

void AObsidianWorldCollectable::AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd)
{
	check(InstanceToAdd);
	StaticContent.Instances.Add(FPickupInstance(InstanceToAdd));
}

void AObsidianWorldCollectable::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 ItemStacks)
{
	check(ItemDef);
	StaticContent.Templates.Add(FPickupTemplate(ItemDef, ItemStacks));
}

bool AObsidianWorldCollectable::CarriesItemInstance() const
{
	const FPickupContent PickupContent = GetPickupContent();
	if(PickupContent.Instances.Num() > 0)
	{
		return true;
	}
	return false;
}

bool AObsidianWorldCollectable::CarriesItemDef() const
{
	const FPickupContent PickupContent = GetPickupContent();
	if(PickupContent.Templates.Num() > 0)
	{
		return true;
	}
	return false;
}

bool AObsidianWorldCollectable::CarriesBoth() const
{
	const FPickupContent PickupContent = GetPickupContent();
	if((PickupContent.Instances.Num() > 0) && (PickupContent.Templates.Num() > 0))
	{
		return true;
	}
	return false;
}



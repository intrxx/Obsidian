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

FPickupInstance AObsidianWorldCollectable::GetPickupInstanceFromPickupContent() const
{
	FPickupInstance PickupInstance = GetPickupContent().Instance;
	if(!PickupInstance.IsValid())
	{
		return FPickupInstance(nullptr);
	}
	return PickupInstance;
}

FPickupTemplate AObsidianWorldCollectable::GetPickupTemplateFromPickupContent() const
{
	FPickupTemplate PickupTemplate = GetPickupContent().Template;
	if(!PickupTemplate.IsValid())
	{
		return FPickupTemplate(nullptr, -1);
	}
	return PickupTemplate;
}

void AObsidianWorldCollectable::AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd)
{
	checkf(InstanceToAdd, TEXT("Provided InstanceToAdd is invalid in AObsidianWorldCollectable::AddItemInstance."));
	StaticContent.Instance = FPickupInstance(InstanceToAdd);
}

void AObsidianWorldCollectable::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 ItemStacks)
{
	checkf(ItemDef, TEXT("Provided ItemDef is invalid in AObsidianWorldCollectable::AddItemDefinition."));
	StaticContent.Template = FPickupTemplate(ItemDef, ItemStacks);
}

void AObsidianWorldCollectable::OverrideTemplateStacks(const int32 NewItemStacks)
{
	if(StaticContent.Template.IsValid())
	{
		StaticContent.Template.StackCount = NewItemStacks;
	}
}

bool AObsidianWorldCollectable::CarriesItemInstance() const
{
	if(const FPickupContent PickupContent = GetPickupContent(); PickupContent.Instance.IsValid())
	{
		return true;
	}
	return false;
}

bool AObsidianWorldCollectable::CarriesItemDef() const
{
	if(const FPickupContent PickupContent = GetPickupContent(); PickupContent.Template.IsValid())
	{
		return true;
	}
	return false;
}

bool AObsidianWorldCollectable::CarriesBoth() const
{
	const FPickupContent PickupContent = GetPickupContent();
	if((PickupContent.Instance.IsValid()) && (PickupContent.Template.IsValid()))
	{
		return true;
	}
	return false;
}



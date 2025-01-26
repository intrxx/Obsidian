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

FPickupInstance AObsidianWorldCollectable::GetFirstPickupInstanceFromPickupContent() const
{
	TArray<FPickupInstance> PickupInstances = GetPickupContent().Instances;
	if(PickupInstances.IsEmpty())
	{
		return FPickupInstance(nullptr);
	}

	FPickupInstance PickupInstance = PickupInstances[0];
	if(PickupInstance.Item)
	{
		return PickupInstance;
	}
	return FPickupInstance(nullptr);
}

FPickupTemplate AObsidianWorldCollectable::GetFirstPickupTemplateFromPickupContent() const
{
	TArray<FPickupTemplate> PickupTemplates = GetPickupContent().Templates;
	if(PickupTemplates.IsEmpty())
	{
		return FPickupTemplate(nullptr, -1);
	}

	FPickupTemplate PickupTemplate = PickupTemplates[0];
	if(PickupTemplate.ItemDef)
	{
		return PickupTemplate;
	}
	return FPickupTemplate(nullptr, -1);
}

void AObsidianWorldCollectable::AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd)
{
	checkf(InstanceToAdd, TEXT("Provided InstanceToAdd is invalid in AObsidianWorldCollectable::AddItemInstance."));
	StaticContent.Instances.Add(FPickupInstance(InstanceToAdd));
}

void AObsidianWorldCollectable::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 ItemStacks)
{
	checkf(ItemDef, TEXT("Provided ItemDef is invalid in AObsidianWorldCollectable::AddItemDefinition."));
	StaticContent.Templates.Add(FPickupTemplate(ItemDef, ItemStacks));
}

void AObsidianWorldCollectable::OverrideTemplateStacks(const int32 NewItemStacks, const int32 TemplateIndex)
{
	if(StaticContent.Templates[TemplateIndex].ItemDef)
	{
		StaticContent.Templates[TemplateIndex].StackCount = NewItemStacks;
	}
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



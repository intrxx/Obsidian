// Copyright 2024 out of sCope team - Michał Ogiński


#include "Gameplay//ObsidianWorldCollectable.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

AObsidianWorldCollectable::AObsidianWorldCollectable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AObsidianWorldCollectable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PickupContent);
}

FPickupContent AObsidianWorldCollectable::GetPickupContent() const
{
	return PickupContent;
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
	PickupContent.Instance = FPickupInstance(InstanceToAdd);
	
	if(InstanceToAdd && IsUsingRegisteredSubObjectList())
	{
		AddReplicatedSubObject(InstanceToAdd);
	}
}

void AObsidianWorldCollectable::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 ItemStacks)
{
	checkf(ItemDef, TEXT("Provided ItemDef is invalid in AObsidianWorldCollectable::AddItemDefinition."));
	PickupContent.Template = FPickupTemplate(ItemDef, ItemStacks);

	if(ItemDef && IsUsingRegisteredSubObjectList())
	{
		AddReplicatedSubObject(ItemDef);
	}
}

void AObsidianWorldCollectable::OverrideTemplateStacks(const int32 NewItemStacks)
{
	if(PickupContent.Template.IsValid())
	{
		PickupContent.Template.StackCount = NewItemStacks;
	}
}

bool AObsidianWorldCollectable::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething =  Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	UObsidianInventoryItemInstance* Instance = PickupContent.Instance.Item;
	if(Instance && IsValid(Instance))
	{
		WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
	}
	
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = PickupContent.Template.ItemDef;
	if(ItemDef && IsValid(ItemDef))
	{
		WroteSomething |= Channel->ReplicateSubobject(ItemDef, *Bunch, *RepFlags);
	}

	return WroteSomething;
}

void AObsidianWorldCollectable::OnRep_PickupContent()
{
	
}

bool AObsidianWorldCollectable::CarriesItemInstance() const
{
	if(PickupContent.Instance.IsValid())
	{
		return true;
	}
	return false;
}

bool AObsidianWorldCollectable::CarriesItemDef() const
{
	if(PickupContent.Template.IsValid())
	{
		return true;
	}
	return false;
}

bool AObsidianWorldCollectable::CarriesBoth() const
{
	const FPickupContent Content = GetPickupContent();
	if((Content.Instance.IsValid()) && (Content.Template.IsValid()))
	{
		return true;
	}
	return false;
}



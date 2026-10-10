// Copyright 2026 out of sCope team - intrxx

#include "Gameplay/ObsidianWorldCollectable.h"

#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"


AObsidianWorldCollectable::AObsidianWorldCollectable(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
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

FObsidianPickupContent AObsidianWorldCollectable::GetPickupContent() const
{
	return PickupContent;
}

FObsidianPickupInstance AObsidianWorldCollectable::GetPickupInstanceFromPickupContent() const
{
	FObsidianPickupInstance PickupInstance = GetPickupContent().Instance;
	if(!PickupInstance.IsValid())
	{
		return FObsidianPickupInstance(nullptr);
	}
	return PickupInstance;
}

FObsidianPickupTemplate AObsidianWorldCollectable::GetPickupTemplateFromPickupContent() const
{
	FObsidianPickupTemplate PickupTemplate = GetPickupContent().Template;
	if(!PickupTemplate.IsValid())
	{
		return FObsidianPickupTemplate(nullptr, -1);
	}
	return PickupTemplate;
}

void AObsidianWorldCollectable::AddItemInstance(UObsidianInventoryItemInstance* InInstanceToAdd)
{
	checkf(InInstanceToAdd, TEXT("Provided InstanceToAdd is invalid in AObsidianWorldCollectable::AddItemInstance."));
	PickupContent.Instance = FObsidianPickupInstance(InInstanceToAdd);
	
	if(InInstanceToAdd && IsUsingRegisteredSubObjectList())
	{
		AddReplicatedSubObject(InInstanceToAdd);
	}
}

void AObsidianWorldCollectable::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> InItemDef,
	const FObsidianItemGeneratedData& InGeneratedData)
{
	checkf(InItemDef, TEXT("Provided ItemDef is invalid in AObsidianWorldCollectable::AddItemDefinition."));
	PickupContent.Template = FObsidianPickupTemplate(InItemDef, InGeneratedData);

	if(InItemDef && IsUsingRegisteredSubObjectList())
	{
		AddReplicatedSubObject(InItemDef);
	}
}

void AObsidianWorldCollectable::OverrideTemplateStacks(const int32 InNewItemStacks)
{
	if(PickupContent.Template.IsValid())
	{
		PickupContent.Template.ItemGeneratedData.SetStackCount(InNewItemStacks);
	}
}

bool AObsidianWorldCollectable::ReplicateSubobjects(UActorChannel* InChannel, FOutBunch* InBunch, FReplicationFlags* InRepFlags)
{
	bool WroteSomething =  Super::ReplicateSubobjects(InChannel, InBunch, InRepFlags);

	UObsidianInventoryItemInstance* Instance = PickupContent.Instance.Item;
	if(Instance && IsValid(Instance))
	{
		WroteSomething |= InChannel->ReplicateSubobject(Instance, *InBunch, *InRepFlags);
	}
	
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = PickupContent.Template.ItemDef;
	if(ItemDef && IsValid(ItemDef))
	{
		WroteSomething |= InChannel->ReplicateSubobject(ItemDef, *InBunch, *InRepFlags);
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
	const FObsidianPickupContent Content = GetPickupContent();
	if((Content.Instance.IsValid()) && (Content.Template.IsValid()))
	{
		return true;
	}
	return false;
}



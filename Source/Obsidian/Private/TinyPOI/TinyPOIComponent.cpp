// Copyright 2026 out of sCope team - intrxx

#include "TinyPOI/TinyPOIComponent.h"

#include "Engine/World.h"
#include "Misc/Crc.h"

#include "TinyPOI/TinyPOISubsystem.h"


UTinyPOIComponent::UTinyPOIComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

#if WITH_EDITOR
	// Moving the component (or anything it is attached to) changes the baked Point.
	bWantsOnUpdateTransform = true;
#endif

#if WITH_EDITORONLY_DATA
	// Shows the default sprite, so the component can be seen and picked in the level.
	bVisualizeComponent = true;
#endif
}

void UTinyPOIComponent::SetTag(const FGameplayTag& InTag)
{
	Tag = InTag;

#if WITH_EDITOR
	NotifyBuilderOfChange();
#endif
}

void UTinyPOIComponent::SetRadius(const float InRadius)
{
	Radius = FMath::Max(0.0f, InRadius);

#if WITH_EDITOR
	NotifyBuilderOfChange();
#endif
}

#if WITH_EDITOR
uint32 UTinyPOIComponent::GetPOIHash() const
{
	// Every property the Builder reads needs to be a part of the hash, otherwise changing it will not cause a bake.
	// ZoneGraph avoids listing them by hashing every property marked with the IncludeInHash meta (FZoneGraphObjectCRC32).
	uint32 Hash = GetTypeHash(GetComponentTransform());
	Hash = HashCombine(Hash, GetTypeHash(Radius));

	// The hash of an FName changes between sessions, the hash of its string does not.
	Hash = HashCombine(Hash, FCrc::StrCrc32(*Tag.ToString()));
	return Hash;
}

void UTinyPOIComponent::PostEditChangeProperty(FPropertyChangedEvent& InPropertyChangedEvent)
{
	Super::PostEditChangeProperty(InPropertyChangedEvent);

	NotifyBuilderOfChange();
}

void UTinyPOIComponent::PostEditUndo()
{
	Super::PostEditUndo();

	NotifyBuilderOfChange();
}

void UTinyPOIComponent::NotifyBuilderOfChange()
{
	if(IsRegistered() == false)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if(World && World->IsGameWorld() == false)
	{
		if(UTinyPOISubsystem* POISubsystem = World->GetSubsystem<UTinyPOISubsystem>())
		{
			POISubsystem->GetBuilder().OnPOIComponentChanged(*this);
		}
	}
}
#endif // WITH_EDITOR

void UTinyPOIComponent::OnRegister()
{
	Super::OnRegister();

#if WITH_EDITOR
	// The bake only happens in the editor world, the markup is not a part of the game, PIE included.
	const UWorld* World = GetWorld();
	if(World && World->IsGameWorld() == false)
	{
		if(UTinyPOISubsystem* POISubsystem = World->GetSubsystem<UTinyPOISubsystem>())
		{
			POISubsystem->GetBuilder().RegisterPOIComponent(*this);
		}
	}
#endif
}

void UTinyPOIComponent::OnUnregister()
{
#if WITH_EDITOR
	const UWorld* World = GetWorld();
	if(World && World->IsGameWorld() == false)
	{
		if(UTinyPOISubsystem* POISubsystem = World->GetSubsystem<UTinyPOISubsystem>())
		{
			POISubsystem->GetBuilder().UnregisterPOIComponent(*this);
		}
	}
#endif

	Super::OnUnregister();
}

void UTinyPOIComponent::OnUpdateTransform(EUpdateTransformFlags InUpdateTransformFlags, ETeleportType InTeleport)
{
	Super::OnUpdateTransform(InUpdateTransformFlags, InTeleport);

#if WITH_EDITOR
	NotifyBuilderOfChange();
#endif
}

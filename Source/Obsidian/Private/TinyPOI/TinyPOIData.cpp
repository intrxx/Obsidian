// Copyright 2026 out of sCope team - intrxx

#include "TinyPOI/TinyPOIData.h"

#include "Engine/Level.h"
#include "Engine/World.h"

#include "Obsidian/ObsidianLogCategories.h"
#include "TinyPOI/TinyPOISubsystem.h"


ATinyPOIData::ATinyPOIData(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	SetCanBeDamaged(false);

#if WITH_EDITORONLY_DATA
	// The baked data covers the whole level, it can not be streamed in and out with a part of it.
	bIsSpatiallyLoaded = false;
#endif
}

void ATinyPOIData::OnRegistered(const FTinyPOIDataHandle InDataHandle)
{
	Storage.DataHandle = InDataHandle;
	bRegistered = true;
}

void ATinyPOIData::OnUnregistered()
{
	Storage.DataHandle.Reset();
	bRegistered = false;
}

/**
 * There is no single callback that covers every way a level can join or leave a world, these are the cases
 * AZoneGraphData lists and handles:
 *
 *   Level loaded with the world (editor, PIE, game)     PostLoad(), fails as the Subsystem does not exist yet,
 *                                                        UTinyPOISubsystem::PostInitialize() registers what was missed
 *   Sub level loaded or shown                            PostLoad(), PreRegisterAllComponents()
 *   Sub level hidden or unloaded                         PostUnregisterAllComponents(), EndPlay()
 *   Spawned by the Subsystem                             PostActorCreated()
 *   Deleted in the editor                                Destroyed()
 *   Undo and redo of the above                           PostEditUndo()
 *   End of the game or PIE                               EndPlay()
 *
 * Several of them fire in a row for the same event, bRegistered makes all but the first one do nothing.
 */

void ATinyPOIData::PostLoad()
{
	Super::PostLoad();

	const bool bSucceeded = RegisterWithSubsystem();
	UE_LOG(ObLogTinyPOI, Verbose, TEXT("[%s] PostLoad - %s."), *GetName(), bSucceeded ? TEXT("registered") : TEXT("not registered"));
}

#if WITH_EDITOR
void ATinyPOIData::PostEditUndo()
{
	Super::PostEditUndo();

	const bool bSucceeded = IsPendingKillPending() ? UnregisterWithSubsystem() : RegisterWithSubsystem();
	UE_LOG(ObLogTinyPOI, Verbose, TEXT("[%s] PostEditUndo - %s."), *GetName(), bSucceeded ? TEXT("changed") : TEXT("not changed"));
}
#endif

void ATinyPOIData::PostActorCreated()
{
	Super::PostActorCreated();

	const bool bSucceeded = RegisterWithSubsystem();
	UE_LOG(ObLogTinyPOI, Verbose, TEXT("[%s] PostActorCreated - %s."), *GetName(), bSucceeded ? TEXT("registered") : TEXT("not registered"));
}

void ATinyPOIData::PreRegisterAllComponents()
{
	Super::PreRegisterAllComponents();

	// This is called in the editor all the time, the only interesting call is the one made when the level is being
	// added to the world.
	const ULevel* Level = GetLevel();
	if(Level && Level->bIsAssociatingLevel)
	{
		const bool bSucceeded = RegisterWithSubsystem();
		UE_LOG(ObLogTinyPOI, Verbose, TEXT("[%s] PreRegisterAllComponents - %s."), *GetName(), bSucceeded ? TEXT("registered") : TEXT("not registered"));
	}
}

void ATinyPOIData::PostUnregisterAllComponents()
{
	// Same as above, only interested in the level being removed from the world.
	const ULevel* Level = GetLevel();
	if(Level && Level->bIsDisassociatingLevel)
	{
		const bool bSucceeded = UnregisterWithSubsystem();
		UE_LOG(ObLogTinyPOI, Verbose, TEXT("[%s] PostUnregisterAllComponents - %s."), *GetName(), bSucceeded ? TEXT("unregistered") : TEXT("not unregistered"));
	}

	Super::PostUnregisterAllComponents();
}

void ATinyPOIData::Destroyed()
{
	const bool bSucceeded = UnregisterWithSubsystem();
	UE_LOG(ObLogTinyPOI, Verbose, TEXT("[%s] Destroyed - %s."), *GetName(), bSucceeded ? TEXT("unregistered") : TEXT("not unregistered"));

	Super::Destroyed();
}

void ATinyPOIData::EndPlay(const EEndPlayReason::Type InEndPlayReason)
{
	const bool bSucceeded = UnregisterWithSubsystem();
	UE_LOG(ObLogTinyPOI, Verbose, TEXT("[%s] EndPlay - %s."), *GetName(), bSucceeded ? TEXT("unregistered") : TEXT("not unregistered"));

	Super::EndPlay(InEndPlayReason);
}

bool ATinyPOIData::RegisterWithSubsystem()
{
	if(bRegistered || HasAnyFlags(RF_ClassDefaultObject))
	{
		return false;
	}

	if(UTinyPOISubsystem* POISubsystem = UWorld::GetSubsystem<UTinyPOISubsystem>(GetWorld()))
	{
		return POISubsystem->RegisterPOIData(*this).IsValid();
	}
	return false;
}

bool ATinyPOIData::UnregisterWithSubsystem()
{
	if(bRegistered == false)
	{
		return false;
	}

	if(UTinyPOISubsystem* POISubsystem = UWorld::GetSubsystem<UTinyPOISubsystem>(GetWorld()))
	{
		POISubsystem->UnregisterPOIData(*this);
		return true;
	}
	return false;
}

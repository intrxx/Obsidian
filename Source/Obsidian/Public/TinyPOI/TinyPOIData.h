// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "TinyPOI/TinyPOITypes.h"

#include "TinyPOIData.generated.h"

/**
 * Holds the baked Points Of Interest of one level. This Actor is what carries the baked data from the editor to the
 * game: the Storage is a regular property, so it is saved with the level and is back as soon as the level is loaded.
 *
 * It is never placed by hand, the UTinyPOISubsystem spawns one in every level that has UTinyPOIComponents in it. The
 * only thing it does on its own is registering in the Subsystem whenever its level joins a world and unregistering
 * when it leaves.
 */
UCLASS(NotBlueprintable, NotPlaceable)
class OBSIDIAN_API ATinyPOIData : public AActor
{
	GENERATED_BODY()

public:
	ATinyPOIData(const FObjectInitializer& InObjectInitializer);

	bool IsRegistered() const { return bRegistered; }

	/** Called by the UTinyPOISubsystem when it registers or unregisters this Data. */
	void OnRegistered(const FTinyPOIDataHandle InDataHandle);
	void OnUnregistered();

	const FTinyPOIStorage& GetStorage() const { return Storage; }
	FTinyPOIStorage& GetStorageMutable() { return Storage; }

	/** Combined hash of all the UTinyPOIComponents the Storage was baked from. */
	uint32 GetCombinedHash() const { return CombinedHash; }
	void SetCombinedHash(const uint32 InCombinedHash) { CombinedHash = InCombinedHash; }

	//~ Begin UObject Interface
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditUndo() override;
#endif
	//~ End UObject Interface

	//~ Begin AActor Interface
	virtual void PostActorCreated() override;
	virtual void PreRegisterAllComponents() override;
	virtual void PostUnregisterAllComponents() override;
	virtual void Destroyed() override;
	virtual void EndPlay(const EEndPlayReason::Type InEndPlayReason) override;
#if WITH_EDITOR
	virtual bool CanChangeIsSpatiallyLoadedFlag() const override { return false; }
#endif
	//~ End AActor Interface

protected:
	bool RegisterWithSubsystem();
	bool UnregisterWithSubsystem();

private:
	/** The baked data. */
	UPROPERTY()
	FTinyPOIStorage Storage;

	/** What the markup hashed to when the Storage was baked, the next bake is skipped for as long as it stays the same. */
	UPROPERTY()
	uint32 CombinedHash = 0;

	bool bRegistered = false;
};

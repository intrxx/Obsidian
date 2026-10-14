// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#if WITH_EDITOR

class ATinyPOIData;
class ULevel;
class UTinyPOIComponent;
struct FTinyPOIStorage;

/**
 * Bakes the UTinyPOIComponents of a level into the FTinyPOIStorage of its ATinyPOIData. Owned by the UTinyPOISubsystem
 * of the editor world, it does not exist outside of the editor.
 *
 * Components register themselves here and report their changes, which only marks the Builder as dirty. The actual
 * bake is started by the Subsystem and skips every Data whose markup hashes to the value it was last baked with, so
 * loading a level that is up to date does not touch (and dirty) it.
 */
class OBSIDIAN_API FTinyPOIBuilder
{
public:
	void RegisterPOIComponent(UTinyPOIComponent& InComponent);
	void UnregisterPOIComponent(UTinyPOIComponent& InComponent);
	void OnPOIComponentChanged(UTinyPOIComponent& InComponent);

	TConstArrayView<TWeakObjectPtr<UTinyPOIComponent>> GetRegisteredPOIComponents() const { return Components; }

	bool NeedsRebuild() const { return bIsDirty; }

	/**
	 * Bakes every Data whose markup changed since it was last baked.
	 * @param InAllPOIData All the Data to update, each one is baked from the components of its own level.
	 * @param bInForceRebuild Bakes even if the markup did not change.
	 */
	void BuildAll(const TConstArrayView<ATinyPOIData*> InAllPOIData, const bool bInForceRebuild);

	/** Bakes the components of the level into the Storage, with no hash check involved. */
	void BuildStorage(const ULevel* InLevel, FTinyPOIStorage& OutStorage) const;

private:
	/**
	 * Hashed together with the markup. Bump it when BuildStorage() changes what it outputs, so every level gets baked
	 * again the next time it is opened. (ZoneGraph hashes its build settings for the same reason.)
	 */
	static constexpr uint32 BuildVersion = 1;

	/** Hash of all the markup in the level, this is what gets compared against ATinyPOIData::GetCombinedHash(). */
	uint32 CalculateCombinedHash(const ULevel* InLevel) const;

	TArray<TWeakObjectPtr<UTinyPOIComponent>> Components;

	bool bIsDirty = false;
};

#endif // WITH_EDITOR

// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"

#include "Mass/ObsidianMassEnemyFragments.h"

#include "ObsidianMassEnemyTrait.generated.h"

/**
 * Makes the Mass entity an Obsidian Enemy - gives it the identity, the combat target and the state that is shared
 * with the Actor the entity is hydrated into.
 */
UCLASS(meta = (DisplayName = "Obsidian Enemy"))
class OBSIDIAN_API UObsidianMassEnemyTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	//~ Begin UMassEntityTraitBase Interface
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& InOutBuildContext, const UWorld& InWorld) const override;
	//~ End UMassEntityTraitBase Interface

	/** Identity the entities start with, the ones created out of the placed Enemies take it from them instead. */
	UPROPERTY(EditAnywhere, Category = "Obsidian")
	FObsidianMassEnemyIdentityFragment DefaultIdentity;

	UPROPERTY(EditAnywhere, Category = "Obsidian")
	FObsidianMassEnemyParameters Parameters;
};

// Copyright 2026 out of sCope team - intrxx

#include "Mass/ObsidianMassEnemyTrait.h"

#include "MassActorSubsystem.h"
#include "MassCommonFragments.h"
#include "MassEntityManager.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntityUtils.h"


void UObsidianMassEnemyTrait::BuildTemplate(FMassEntityTemplateBuildContext& InOutBuildContext, const UWorld& InWorld) const
{
	InOutBuildContext.RequireFragment<FTransformFragment>();
	InOutBuildContext.RequireFragment<FMassActorFragment>();

	InOutBuildContext.AddTag<FObsidianMassEnemyTag>();
	InOutBuildContext.AddFragment_GetRef<FObsidianMassEnemyIdentityFragment>() = DefaultIdentity;
	InOutBuildContext.AddFragment<FObsidianMassCombatTargetFragment>();
	InOutBuildContext.AddFragment<FObsidianMassEnemyStateFragment>();

	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(InWorld);
	InOutBuildContext.AddConstSharedFragment(EntityManager.GetOrCreateConstSharedFragment(Parameters));
}

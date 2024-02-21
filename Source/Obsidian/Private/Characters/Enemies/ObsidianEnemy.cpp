// Copyright 2024 Michał Ogiński

#include "Characters/Enemies/ObsidianEnemy.h"
#include "ObsidianTypes/ObsidianChannels.h"
#include "ObsidianTypes/ObsidianStencilValues.h"

AObsidianEnemy::AObsidianEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(Obsidian_TraceChannel_Highlight, ECR_Block);
	GetMesh()->SetCustomDepthStencilValue(ObsidianHighlight::Red);
	GetMesh()->SetRenderCustomDepth(false);
}

void AObsidianEnemy::StartHighlight()
{
	GetMesh()->SetRenderCustomDepth(true);
}

void AObsidianEnemy::StopHighlight()
{
	GetMesh()->SetRenderCustomDepth(false);
}

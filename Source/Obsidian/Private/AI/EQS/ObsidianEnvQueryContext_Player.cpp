// Copyright 2024 out of sCope team - Michał Ogiński


#include "AI/EQS/ObsidianEnvQueryContext_Player.h"

#include "Characters/Heroes/ObsidianHero.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Kismet/GameplayStatics.h"

void UObsidianEnvQueryContext_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	TArray<AActor*> Heroes;

	UGameplayStatics::GetAllActorsOfClass(this, AObsidianHero::StaticClass(), Heroes);

	if(!Heroes.IsEmpty())
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, Heroes);
	}
}

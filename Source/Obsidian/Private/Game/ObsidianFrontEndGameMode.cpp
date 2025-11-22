// Copyright 2024 out of sCope team - Michał Ogiński

#include "Game/ObsidianFrontEndGameMode.h"


#include "Characters/Player/ObsidianPlayerController.h"

void FObsidianHeroClassParams::Reset()
{
	ObsidianPlayerName = FText();
	bIsHardcore = false;
	bIsOnline = false;
	HeroObjectClass = nullptr;
	Class = EObsidianHeroClass::None;
	TempID = 0;
}

bool FObsidianHeroClassParams::IsValid() const
{
	return HeroObjectClass.IsNull() == false;
}

AObsidianFrontEndGameMode::AObsidianFrontEndGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerControllerClass = AObsidianPlayerController::StaticClass();
}

FObsidianHeroClassParams AObsidianFrontEndGameMode::CreateHeroClass(const EObsidianHeroClass InClass, const FText& InName,
	const bool InIsOnline, const bool InIsHardcore)
{
	if(InName.IsEmpty() || InClass == EObsidianHeroClass::None)
	{
		return FObsidianHeroClassParams();
	}
	
	check(ClassToSoftClassMap.Contains(InClass));
	
	FObsidianHeroClassParams HeroClassParams = FObsidianHeroClassParams();
	HeroClassParams.HeroObjectClass = ClassToSoftClassMap[InClass];
	HeroClassParams.Class = InClass;
	HeroClassParams.ObsidianPlayerName = InName;
	HeroClassParams.bIsOnline = InIsOnline;
	HeroClassParams.bIsHardcore = InIsHardcore;
	HeroClassParams.TempID = CreatedHeroes.Num();
	
	CreatedHeroes.Add(HeroClassParams);
	return HeroClassParams;
}

bool AObsidianFrontEndGameMode::DeleteHeroClass(const int32 WithID)
{
	for(auto It = CreatedHeroes.CreateIterator(); It; ++It)
	{
		FObsidianHeroClassParams& Params = *It;
		if(Params.TempID == WithID)
		{
			It.RemoveCurrent();
			return true;
		}
	}
	return false;
}


	

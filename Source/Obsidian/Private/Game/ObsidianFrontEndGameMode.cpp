// Copyright 2026 out of sCope team - intrxx

#include "Game/ObsidianFrontEndGameMode.h"

#include "Kismet/GameplayStatics.h"

#include "Characters/Player/ObsidianLocalPlayer.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Game/Save/ObsidianSaveGameSubsystem.h"


void FObsidianHeroClassParams::Reset()
{
	ObsidianPlayerName = FString();
	bIsHardcore = false;
	bIsOnline = false;
	HeroObjectClass = nullptr;
	Class = EObsidianHeroClass::None;
	HeroID = 0;
}

bool FObsidianHeroClassParams::IsValid() const
{
	return HeroObjectClass.IsNull() == false;
}

AObsidianFrontEndGameMode::AObsidianFrontEndGameMode(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	PlayerControllerClass = AObsidianPlayerController::StaticClass();
}

FObsidianHeroClassParams AObsidianFrontEndGameMode::CreateHeroClass(const EObsidianHeroClass InClass, const FString& InName,
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
	
	CreatedHeroes.Add(HeroClassParams);
	return HeroClassParams;
}

bool AObsidianFrontEndGameMode::DeleteHeroClass(const int32 InWithID)
{
	for(auto It = CreatedHeroes.CreateIterator(); It; ++It)
	{
		FObsidianHeroClassParams& Params = *It;
		if(Params.HeroID == InWithID)
		{
			It.RemoveCurrent();
			return true;
		}
	}
	return false;
}

void AObsidianFrontEndGameMode::BeginPlay()
{
	Super::BeginPlay();

	//TODO(intrxx) call in some better place, possibly earlier
	UObsidianSaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<UObsidianSaveGameSubsystem>();
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	SaveGameSubsystem->LoadOrCreateMasterSaveObject(Cast<UObsidianLocalPlayer>(PlayerController->GetLocalPlayer()));
}


	

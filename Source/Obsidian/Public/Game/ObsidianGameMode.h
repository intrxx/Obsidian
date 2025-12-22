// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include "ObsidianTypes/ObsidianCoreTypes.h"

#include <GameFramework/GameModeBase.h>
#include "ObsidianGameMode.generated.h"

class AObsidianTownPortal;
class UObsidianEnemyTypeInfo;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	EObsidianGameNetworkType GetCurrentNetworkType() const;
	UObsidianEnemyTypeInfo* GetEnemyTypeInfo() const;

	void RegisterPortal(AObsidianTownPortal* InNewTownPortal);
	bool CanCreatePortal() const;

protected:
	UPROPERTY()
	EObsidianGameNetworkType CurrentNetworkType = EObsidianGameNetworkType::None;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UObsidianEnemyTypeInfo> EnemyTypeInfo;

	UPROPERTY()
	TArray<TObjectPtr<AObsidianTownPortal>> ActiveTownPortals;
};

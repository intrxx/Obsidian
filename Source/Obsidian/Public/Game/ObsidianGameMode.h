// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "ObsidianTypes/ObsidianCoreTypes.h"

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
	EObsidianGameNetworkType GetCurrentNetworkType() const { return CurrentNetworkType; }
	UObsidianEnemyTypeInfo* GetEnemyTypeInfo() const { return EnemyTypeInfo; }

	void RegisterPortal(AObsidianTownPortal* InNewTownPortal);
	bool CanCreatePortal() const;

	//~ Begin AGameModeBase Interface
	virtual void InitGame(const FString& InMapName, const FString& InOptions, FString& OutErrorMessage) override;
	//~ End AGameModeBase Interface

protected:
	UPROPERTY()
	EObsidianGameNetworkType CurrentNetworkType = EObsidianGameNetworkType::None;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UObsidianEnemyTypeInfo> EnemyTypeInfo;

	UPROPERTY()
	TArray<TObjectPtr<AObsidianTownPortal>> ActiveTownPortals;
};

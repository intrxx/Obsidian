// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ObsidianGameMode.generated.h"

class UObsidianEnemyTypeInfo;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	//~AGameModeBase interface
	//~End of AGameModeBase interface
public:
	UObsidianEnemyTypeInfo* GetEnemyTypeInfo() const {return EnemyTypeInfo;}
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UObsidianEnemyTypeInfo> EnemyTypeInfo;
};

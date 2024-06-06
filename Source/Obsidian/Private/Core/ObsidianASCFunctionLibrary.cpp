// Copyright 2024 out of sCope team - Michał Ogiński


#include "Core/ObsidianASCFunctionLibrary.h"
#include "AbilitySystem/Data/ObsidianAbilitySet.h"
#include "AbilitySystem/Data/ObsidianEnemyTypeInfo.h"
#include "Game/ObsidianGameMode.h"
#include "Kismet/GameplayStatics.h"

void UObsidianASCFunctionLibrary::InitializeEnemyDefaultAttributesWithClass(const UObject* WorldContextObject, UObsidianAbilitySystemComponent* ASC,
                                                                   const EObsidianEnemyClass EnemyClass, const float Level, UObject* SourceObject)
{
    if(WorldContextObject == nullptr)
    {
        return;
    }
	
    if(const AObsidianGameMode* ObsidianGameMode = Cast<AObsidianGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
    {
        FObsidianEnemyTypeDefaultInfo EnemyDefaultInfo = ObsidianGameMode->GetEnemyTypeInfo()->GetEnemyTypeDefaultInfo(EnemyClass);

        if(EnemyDefaultInfo.DefaultAbilitySet)
        {
            EnemyDefaultInfo.DefaultAbilitySet->GiveToAbilitySystem(ASC, nullptr, Level, SourceObject);
        }
    }
}

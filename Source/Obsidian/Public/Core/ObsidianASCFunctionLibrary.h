// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ObsidianTypes/Character/ObsidianEnemyType.h"
#include "ObsidianASCFunctionLibrary.generated.h"

class UObsidianAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianASCFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "ObsidianUIFunctionLibrary|WidgetControllers")
	static void InitializeEnemyDefaultAttributesWithClass(const UObject* WorldContextObject, UObsidianAbilitySystemComponent* ASC, const EObsidianEnemyClass EnemyClass, const float Level, UObject* SourceObject = nullptr);
};

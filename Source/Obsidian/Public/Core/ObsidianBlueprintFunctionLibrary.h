// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ObsidianBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category = "ObsidianBlueprintFunctionLibrary")
	static bool IsActorOfClass(const AActor* ActorToCheck, UClass* ActorClass);
};

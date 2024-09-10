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

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3", DevelopmentOnly), Category = "ObsidianBlueprintFunctionLibrary")
	static void PrintVector(const UObject* WorldContextObject, const FVector& VectorToLog, const FString& PrefixMessage = FString("Vector"), const FName Key = NAME_None,
		const float TimeToDisplay = 3.0f, const FLinearColor MessageColor = FLinearColor::Green, const bool bPrintToScreen = true, const bool bPrintToLog = true);
};

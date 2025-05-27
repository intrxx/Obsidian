// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Kismet/BlueprintFunctionLibrary.h"
#include "ObsidianBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "ObsidianBlueprintFunctionLibrary")
	static bool IsActorOfClass(const AActor* ActorToCheck, UClass* ActorClass);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3", DevelopmentOnly), Category = "ObsidianBlueprintFunctionLibrary")
	static void PrintVector3D(const UObject* WorldContextObject, const FVector& VectorToLog, const FString& PrefixMessage = FString("Vector"), const FName Key = NAME_None,
		const float TimeToDisplay = 3.0f, const FLinearColor MessageColor = FLinearColor::Green, const bool bPrintToScreen = true, const bool bPrintToLog = true);
	
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3", DevelopmentOnly), Category = "ObsidianBlueprintFunctionLibrary")
	static void PrintVector2D(const UObject* WorldContextObject, const FVector2D& VectorToLog, const FString& PrefixMessage = FString("Vector"), const FName Key = NAME_None,
		const float TimeToDisplay = 3.0f, const FLinearColor MessageColor = FLinearColor::Green, const bool bPrintToScreen = true, const bool bPrintToLog = true);
};

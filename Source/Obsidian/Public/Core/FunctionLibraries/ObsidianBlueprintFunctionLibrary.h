// Copyright 2026 out of sCope team - intrxx

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

public:
	UFUNCTION(BlueprintPure, Category = "ObsidianBlueprintFunctionLibrary")
	static bool IsActorOfClass(const AActor* InActorToCheck, UClass* InActorClass);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="InWorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3", DevelopmentOnly), Category = "ObsidianBlueprintFunctionLibrary")
	static void PrintVector3D(const UObject* InWorldContextObject, const FVector& InVectorToLog, const FString& InPrefixMessage = FString("Vector"), const FName InKey = NAME_None,
		const float InTimeToDisplay = 3.0f, const FLinearColor InMessageColor = FLinearColor::Green, const bool bInPrintToScreen = true, const bool bInPrintToLog = true);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="InWorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3", DevelopmentOnly), Category = "ObsidianBlueprintFunctionLibrary")
	static void PrintRotator(const UObject* InWorldContextObject, const FRotator& InRotatorToLog, const FString& InPrefixMessage = FString("Rotator"), const FName InKey = NAME_None,
		const float InTimeToDisplay = 3.0f, const FLinearColor InMessageColor = FLinearColor::Green, const bool bInPrintToScreen = true, const bool bInPrintToLog = true);
	
	UFUNCTION(BlueprintCallable, meta=(WorldContext="InWorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "3", DevelopmentOnly), Category = "ObsidianBlueprintFunctionLibrary")
	static void PrintVector2D(const UObject* InWorldContextObject, const FVector2D& InVectorToLog, const FString& InPrefixMessage = FString("Vector"), const FName InKey = NAME_None,
		const float InTimeToDisplay = 3.0f, const FLinearColor InMessageColor = FLinearColor::Green, const bool bInPrintToScreen = true, const bool bInPrintToLog = true);
};

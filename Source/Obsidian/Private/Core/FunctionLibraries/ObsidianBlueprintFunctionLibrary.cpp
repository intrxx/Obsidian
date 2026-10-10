// Copyright 2026 out of sCope team - intrxx

#include "Core/FunctionLibraries/ObsidianBlueprintFunctionLibrary.h"

#include "Engine/Console.h"
#include "Kismet/GameplayStatics.h"

#include "Obsidian/ObsidianLogCategories.h"


bool UObsidianBlueprintFunctionLibrary::IsActorOfClass(const AActor* InActorToCheck, UClass* InActorClass)
{
	if(InActorToCheck == nullptr || InActorClass == nullptr)
	{
		return false;
	}
	
	return InActorToCheck->IsA(InActorClass);
}

void UObsidianBlueprintFunctionLibrary::PrintVector3D(const UObject* InWorldContextObject, const FVector& InVectorToLog, const FString& InPrefixMessage,
	const FName InKey, const float InTimeToDisplay, const FLinearColor InMessageColor, const bool bInPrintToScreen, const bool bInPrintToLog)
{
	const FString FinalStringPrintMessage = FString::Printf(TEXT("[%s]: [X %f,Y %f,Z %f]"), *InPrefixMessage, InVectorToLog.X, InVectorToLog.Y, InVectorToLog.Z);
	
	if(GAreScreenMessagesEnabled && bInPrintToScreen && InTimeToDisplay > 0.0f)
	{
		uint64 InnerKey = -1;
		if(InKey != NAME_None)
		{
			InnerKey = GetTypeHash(InKey);
		}
		GEngine->AddOnScreenDebugMessage(InnerKey, InTimeToDisplay, InMessageColor.ToFColor(true), FinalStringPrintMessage);
	}

	if (bInPrintToLog)
	{
		UE_LOG(ObLogGeneral, Log, TEXT("%s"), *FinalStringPrintMessage);
		
		APlayerController* PC = (InWorldContextObject ? UGameplayStatics::GetPlayerController(InWorldContextObject, 0) : nullptr);
		ULocalPlayer* LocalPlayer = (PC ? Cast<ULocalPlayer>(PC->Player) : nullptr);
		if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->ViewportConsole)
		{
			LocalPlayer->ViewportClient->ViewportConsole->OutputText(FinalStringPrintMessage);
		}
	}
	else
	{
		UE_LOG(ObLogGeneral, Verbose, TEXT("%s"), *FinalStringPrintMessage);
	}
}

void UObsidianBlueprintFunctionLibrary::PrintRotator(const UObject* InWorldContextObject, const FRotator& InRotatorToLog, const FString& InPrefixMessage,
	const FName InKey, const float InTimeToDisplay, const FLinearColor InMessageColor, const bool bInPrintToScreen, const bool bInPrintToLog)
{
	const FString FinalStringPrintMessage = FString::Printf(TEXT("[%s]: [X(Roll) %f,Y(Pitch) %f,Z(Yaw) %f]"), *InPrefixMessage, InRotatorToLog.Roll, InRotatorToLog.Pitch, InRotatorToLog.Yaw);
	
	if(GAreScreenMessagesEnabled && bInPrintToScreen && InTimeToDisplay > 0.0f)
	{
		uint64 InnerKey = -1;
		if(InKey != NAME_None)
		{
			InnerKey = GetTypeHash(InKey);
		}
		GEngine->AddOnScreenDebugMessage(InnerKey, InTimeToDisplay, InMessageColor.ToFColor(true), FinalStringPrintMessage);
	}

	if (bInPrintToLog)
	{
		UE_LOG(ObLogGeneral, Log, TEXT("%s"), *FinalStringPrintMessage);
		
		APlayerController* PC = (InWorldContextObject ? UGameplayStatics::GetPlayerController(InWorldContextObject, 0) : nullptr);
		ULocalPlayer* LocalPlayer = (PC ? Cast<ULocalPlayer>(PC->Player) : nullptr);
		if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->ViewportConsole)
		{
			LocalPlayer->ViewportClient->ViewportConsole->OutputText(FinalStringPrintMessage);
		}
	}
	else
	{
		UE_LOG(ObLogGeneral, Verbose, TEXT("%s"), *FinalStringPrintMessage);
	}
}

void UObsidianBlueprintFunctionLibrary::PrintVector2D(const UObject* InWorldContextObject, const FVector2D& InVectorToLog, const FString& InPrefixMessage,
                                                      const FName InKey, const float InTimeToDisplay, const FLinearColor InMessageColor, const bool bInPrintToScreen, const bool bInPrintToLog)
{
	const FString FinalStringPrintMessage = FString::Printf(TEXT("[%s]: [%f, %f]"), *InPrefixMessage, InVectorToLog.X, InVectorToLog.Y);
	
	if(GAreScreenMessagesEnabled && bInPrintToScreen && InTimeToDisplay > 0.0f)
	{
		uint64 InnerKey = -1;
		if(InKey != NAME_None)
		{
			InnerKey = GetTypeHash(InKey);
		}
		GEngine->AddOnScreenDebugMessage(InnerKey, InTimeToDisplay, InMessageColor.ToFColor(true), FinalStringPrintMessage);
	}

	if (bInPrintToLog)
	{
		UE_LOG(ObLogGeneral, Log, TEXT("%s"), *FinalStringPrintMessage);
		
		APlayerController* PC = (InWorldContextObject ? UGameplayStatics::GetPlayerController(InWorldContextObject, 0) : nullptr);
		ULocalPlayer* LocalPlayer = (PC ? Cast<ULocalPlayer>(PC->Player) : nullptr);
		if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->ViewportConsole)
		{
			LocalPlayer->ViewportClient->ViewportConsole->OutputText(FinalStringPrintMessage);
		}
	}
	else
	{
		UE_LOG(ObLogGeneral, Verbose, TEXT("%s"), *FinalStringPrintMessage);
	}
}

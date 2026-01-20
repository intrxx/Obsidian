// Copyright 2026 out of sCope team - intrxx

#include "Core/FunctionLibraries/ObsidianBlueprintFunctionLibrary.h"

// ~ Core
#include "Engine/Console.h"
#include "Kismet/GameplayStatics.h"

// ~ Project
#include "Obsidian/ObsidianGameModule.h"

bool UObsidianBlueprintFunctionLibrary::IsActorOfClass(const AActor* ActorToCheck, UClass* ActorClass)
{
	if(ActorToCheck == nullptr || ActorClass == nullptr)
	{
		return false;
	}
	
	return ActorToCheck->IsA(ActorClass);
}

void UObsidianBlueprintFunctionLibrary::PrintVector3D(const UObject* WorldContextObject, const FVector& VectorToLog, const FString& PrefixMessage,
	const FName Key, const float TimeToDisplay, const FLinearColor MessageColor, const bool bPrintToScreen, const bool bPrintToLog)
{
	const FString FinalStringPrintMessage = FString::Printf(TEXT("[%s]: [X %f,Y %f,Z %f]"), *PrefixMessage, VectorToLog.X, VectorToLog.Y, VectorToLog.Z);
	
	if(GAreScreenMessagesEnabled && bPrintToScreen && TimeToDisplay > 0.0f)
	{
		uint64 InnerKey = -1;
		if(Key != NAME_None)
		{
			InnerKey = GetTypeHash(Key);
		}
		GEngine->AddOnScreenDebugMessage(InnerKey, TimeToDisplay, MessageColor.ToFColor(true), FinalStringPrintMessage);
	}

	if (bPrintToLog)
	{
		UE_LOG(LogObsidian, Log, TEXT("%s"), *FinalStringPrintMessage);
		
		APlayerController* PC = (WorldContextObject ? UGameplayStatics::GetPlayerController(WorldContextObject, 0) : nullptr);
		ULocalPlayer* LocalPlayer = (PC ? Cast<ULocalPlayer>(PC->Player) : nullptr);
		if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->ViewportConsole)
		{
			LocalPlayer->ViewportClient->ViewportConsole->OutputText(FinalStringPrintMessage);
		}
	}
	else
	{
		UE_LOG(LogObsidian, Verbose, TEXT("%s"), *FinalStringPrintMessage);
	}
}

void UObsidianBlueprintFunctionLibrary::PrintRotator(const UObject* WorldContextObject, const FRotator& RotatorToLog, const FString& PrefixMessage,
	const FName Key, const float TimeToDisplay, const FLinearColor MessageColor, const bool bPrintToScreen, const bool bPrintToLog)
{
	const FString FinalStringPrintMessage = FString::Printf(TEXT("[%s]: [X(Roll) %f,Y(Pitch) %f,Z(Yaw) %f]"), *PrefixMessage, RotatorToLog.Roll, RotatorToLog.Pitch, RotatorToLog.Yaw);
	
	if(GAreScreenMessagesEnabled && bPrintToScreen && TimeToDisplay > 0.0f)
	{
		uint64 InnerKey = -1;
		if(Key != NAME_None)
		{
			InnerKey = GetTypeHash(Key);
		}
		GEngine->AddOnScreenDebugMessage(InnerKey, TimeToDisplay, MessageColor.ToFColor(true), FinalStringPrintMessage);
	}

	if (bPrintToLog)
	{
		UE_LOG(LogObsidian, Log, TEXT("%s"), *FinalStringPrintMessage);
		
		APlayerController* PC = (WorldContextObject ? UGameplayStatics::GetPlayerController(WorldContextObject, 0) : nullptr);
		ULocalPlayer* LocalPlayer = (PC ? Cast<ULocalPlayer>(PC->Player) : nullptr);
		if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->ViewportConsole)
		{
			LocalPlayer->ViewportClient->ViewportConsole->OutputText(FinalStringPrintMessage);
		}
	}
	else
	{
		UE_LOG(LogObsidian, Verbose, TEXT("%s"), *FinalStringPrintMessage);
	}
}

void UObsidianBlueprintFunctionLibrary::PrintVector2D(const UObject* WorldContextObject, const FVector2D& VectorToLog, const FString& PrefixMessage,
                                                      const FName Key, const float TimeToDisplay, const FLinearColor MessageColor, const bool bPrintToScreen, const bool bPrintToLog)
{
	const FString FinalStringPrintMessage = FString::Printf(TEXT("[%s]: [%f, %f]"), *PrefixMessage, VectorToLog.X, VectorToLog.Y);
	
	if(GAreScreenMessagesEnabled && bPrintToScreen && TimeToDisplay > 0.0f)
	{
		uint64 InnerKey = -1;
		if(Key != NAME_None)
		{
			InnerKey = GetTypeHash(Key);
		}
		GEngine->AddOnScreenDebugMessage(InnerKey, TimeToDisplay, MessageColor.ToFColor(true), FinalStringPrintMessage);
	}

	if (bPrintToLog)
	{
		UE_LOG(LogObsidian, Log, TEXT("%s"), *FinalStringPrintMessage);
		
		APlayerController* PC = (WorldContextObject ? UGameplayStatics::GetPlayerController(WorldContextObject, 0) : nullptr);
		ULocalPlayer* LocalPlayer = (PC ? Cast<ULocalPlayer>(PC->Player) : nullptr);
		if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->ViewportConsole)
		{
			LocalPlayer->ViewportClient->ViewportConsole->OutputText(FinalStringPrintMessage);
		}
	}
	else
	{
		UE_LOG(LogObsidian, Verbose, TEXT("%s"), *FinalStringPrintMessage);
	}
}

// Copyright 2024 out of sCope team - Michał Ogiński


#include "Core/ObsidianBlueprintFunctionLibrary.h"
#include "Engine/Console.h"
#include "Kismet/GameplayStatics.h"
#include "Obsidian/ObsidianGameModule.h"

bool UObsidianBlueprintFunctionLibrary::IsActorOfClass(const AActor* ActorToCheck, UClass* ActorClass)
{
	if(ActorToCheck == nullptr || ActorClass == nullptr)
	{
		return false;
	}
	
	return ActorToCheck->IsA(ActorClass);
}

void UObsidianBlueprintFunctionLibrary::PrintVector(const UObject* WorldContextObject, const FVector& VectorToLog, const FString& PrefixMessage,
	const FName Key, const float TimeToDisplay, const FLinearColor MessageColor, const bool bPrintToScreen, const bool bPrintToLog)
{
	const FString FinalStringPrintMessage = FString::Printf(TEXT("[%s]: %f, %f, %f"), *PrefixMessage, VectorToLog.X, VectorToLog.Y, VectorToLog.Z);
	
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

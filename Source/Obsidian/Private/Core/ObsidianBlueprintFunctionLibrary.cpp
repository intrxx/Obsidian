// Copyright 2024 out of sCope team - Michał Ogiński


#include "Core/ObsidianBlueprintFunctionLibrary.h"

bool UObsidianBlueprintFunctionLibrary::IsActorOfClass(const AActor* ActorToCheck, UClass* ActorClass)
{
	if(ActorToCheck == nullptr || ActorClass == nullptr)
	{
		return false;
	}
	
	return ActorToCheck->IsA(ActorClass);
}

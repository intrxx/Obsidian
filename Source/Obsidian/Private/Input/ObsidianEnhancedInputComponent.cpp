// Copyright 2024 Michał Ogiński

#include "Input/ObsidianEnhancedInputComponent.h"

UObsidianEnhancedInputComponent::UObsidianEnhancedInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UObsidianEnhancedInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for(uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
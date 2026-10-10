// Copyright 2026 out of sCope team - intrxx

#include "Input/ObsidianEnhancedInputComponent.h"


UObsidianEnhancedInputComponent::UObsidianEnhancedInputComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

void UObsidianEnhancedInputComponent::RemoveBinds(TArray<uint32>& InOutBindHandles)
{
	for(uint32 Handle : InOutBindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	InOutBindHandles.Reset();
}
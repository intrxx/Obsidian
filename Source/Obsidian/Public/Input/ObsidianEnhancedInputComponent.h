// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "EnhancedInputComponent.h"

#include "ObsidianInputConfig.h"

#include "ObsidianEnhancedInputComponent.generated.h"

class UInputAction;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UObsidianEnhancedInputComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	template<class UserClass, typename FuncType>
	void BindNativeAction(const UObsidianInputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent InTriggerEvent,
		UserClass* InObject, FuncType InFunc, bool bInLogIfNotFound);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UObsidianInputConfig* InInputConfig, UserClass* InObject, PressedFuncType InPressedFunc,
		ReleasedFuncType InReleasedFunc, TArray<uint32>& OutBindHandles);

	void RemoveBinds(TArray<uint32>& InOutBindHandles);
};

template <class UserClass, typename FuncType>
void UObsidianEnhancedInputComponent::BindNativeAction(const UObsidianInputConfig* InInputConfig,
	const FGameplayTag& InInputTag, ETriggerEvent InTriggerEvent, UserClass* InObject, FuncType InFunc, bool bInLogIfNotFound)
{
	check(InInputConfig);

	if(const UInputAction* InputAction = InInputConfig->FindNativeInputActionForTag(InInputTag, bInLogIfNotFound))
	{
		BindAction(InputAction, InTriggerEvent, InObject, InFunc);
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UObsidianEnhancedInputComponent::BindAbilityActions(const UObsidianInputConfig* InInputConfig, UserClass* InObject,
	PressedFuncType InPressedFunc, ReleasedFuncType InReleasedFunc, TArray<uint32>& OutBindHandles)
{
	check(InInputConfig);

	for(const FObsidianInputAction& ObsidianAction : InInputConfig->AbilityInputActions)
	{
		if(ObsidianAction.InputAction && ObsidianAction.InputTag.IsValid())
		{
			if(InPressedFunc)
			{
				OutBindHandles.Add(BindAction(ObsidianAction.InputAction, ETriggerEvent::Triggered, InObject, InPressedFunc,
					ObsidianAction.InputTag).GetHandle());
			}

			if(InReleasedFunc)
			{
				OutBindHandles.Add(BindAction(ObsidianAction.InputAction, ETriggerEvent::Completed, InObject, InReleasedFunc,
					ObsidianAction.InputTag).GetHandle());
			}
		}
	}
}


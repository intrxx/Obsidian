// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core

// ~ Project
#include "ObsidianInputConfig.h"

#include "EnhancedInputComponent.h"
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
	UObsidianEnhancedInputComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	template<class UserClass, typename FuncType>
	void BindNativeAction(const UObsidianInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent,
		UserClass* Object, FuncType Func, bool bLogIfNotFound);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UObsidianInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc,
		ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	void RemoveBinds(TArray<uint32>& BindHandles);
};

template <class UserClass, typename FuncType>
void UObsidianEnhancedInputComponent::BindNativeAction(const UObsidianInputConfig* InputConfig,
	const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);

	if(const UInputAction* InputAction = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(InputAction, TriggerEvent, Object, Func);
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UObsidianEnhancedInputComponent::BindAbilityActions(const UObsidianInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for(const FObsidianInputAction& ObsidianAction : InputConfig->AbilityInputActions)
	{
		if(ObsidianAction.InputAction && ObsidianAction.InputTag.IsValid())
		{
			if(PressedFunc)
			{
				BindHandles.Add(BindAction(ObsidianAction.InputAction, ETriggerEvent::Triggered, Object, PressedFunc,
					ObsidianAction.InputTag).GetHandle());
			}

			if(ReleasedFunc)
			{
				BindHandles.Add(BindAction(ObsidianAction.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc,
					ObsidianAction.InputTag).GetHandle());
			}
		}
	}
}


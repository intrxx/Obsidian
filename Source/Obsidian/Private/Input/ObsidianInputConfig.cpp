// Copyright 2024 out of sCope team - Michał Ogiński


#include "Input/ObsidianInputConfig.h"
#include "Obsidian/ObsidianGameModule.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // ~ With Editor

UObsidianInputConfig::UObsidianInputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UInputAction* UObsidianInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for(const FObsidianInputAction& ObsidianInputAction : NativeInputActions)
	{
		if(ObsidianInputAction.InputAction && (ObsidianInputAction.InputTag == InputTag))
		{
			return ObsidianInputAction.InputAction;
		}
	}

	if(bLogNotFound)
	{
		UE_LOG(LogObsidian, Display, TEXT("Could not find NativeInputAction for [%s] on ObsidianInputConfig [%s]"),
			*InputTag.ToString(), *GetNameSafe(this));
	}
	
	return nullptr;
}

const UInputAction* UObsidianInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for(const FObsidianInputAction& ObsidianInputAction : AbilityInputActions)
	{
		if(ObsidianInputAction.InputAction && (ObsidianInputAction.InputTag == InputTag))
		{
			return ObsidianInputAction.InputAction;
		}
	}

	if(bLogNotFound)
	{
		UE_LOG(LogObsidian, Display, TEXT("Could not find AbilityInputAction for [%s] on ObsidianInputConfig [%s]"),
			*InputTag.ToString(), *GetNameSafe(this));
	}
	
	return nullptr;
}

#if WITH_EDITOR
EDataValidationResult FObsidianInputAction::ValidateData(FDataValidationContext& Context, const int Index, const FString& InputActionsName) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	if(InputAction == nullptr)
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Input Action at index [%i] is null! \n"
			"Please set a valid Input Action class or delete this index entry in the %s Input Actions."), Index, *InputActionsName));

		Context.AddError(ErrorMessage);
	}
	
	if(!InputTag.IsValid())
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Input Tag at index [%i] is invalid! \n"
			"Please set a valid Input Tag or delete this index entry in the %s Input Actions."), Index, *InputActionsName));

		Context.AddError(ErrorMessage);
	}

	return Result;
}

EDataValidationResult UObsidianInputConfig::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	unsigned int NativeIndex = 0;
	for(const FObsidianInputAction& Action : NativeInputActions)
	{
		Result =  CombineDataValidationResults(Result, Action.ValidateData(Context, NativeIndex, FString("Native")));
		NativeIndex++;
	}
	
	unsigned int AbilityIndex = 0;
	for(const FObsidianInputAction& Action : AbilityInputActions)
	{
		Result =  CombineDataValidationResults(Result, Action.ValidateData(Context, AbilityIndex, FString("Ability")));
		AbilityIndex++;
	}

	return Result;
}

#endif // ~ With Editor






// Copyright 2026 out of sCope team - intrxx

#include "Input/ObsidianInputConfig.h"

#include "Obsidian/ObsidianLogCategories.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // ~ With Editor


UObsidianInputConfig::UObsidianInputConfig(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

const UInputAction* UObsidianInputConfig::FindNativeInputActionForTag(const FGameplayTag& InInputTag, bool bInLogNotFound) const
{
	for(const FObsidianInputAction& ObsidianInputAction : NativeInputActions)
	{
		if(ObsidianInputAction.InputAction && (ObsidianInputAction.InputTag == InInputTag))
		{
			return ObsidianInputAction.InputAction;
		}
	}

	if(bInLogNotFound)
	{
		UE_LOG(ObLogInput, Warning, TEXT("Could not find NativeInputAction for [%s] on ObsidianInputConfig [%s]"),
			*InInputTag.ToString(), *GetNameSafe(this));
	}
	
	return nullptr;
}

const UInputAction* UObsidianInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InInputTag, bool bInLogNotFound) const
{
	for(const FObsidianInputAction& ObsidianInputAction : AbilityInputActions)
	{
		if(ObsidianInputAction.InputAction && (ObsidianInputAction.InputTag == InInputTag))
		{
			return ObsidianInputAction.InputAction;
		}
	}

	if(bInLogNotFound)
	{
		UE_LOG(ObLogInput, Warning, TEXT("Could not find AbilityInputAction for [%s] on ObsidianInputConfig [%s]"),
			*InInputTag.ToString(), *GetNameSafe(this));
	}
	
	return nullptr;
}

#if WITH_EDITOR
EDataValidationResult FObsidianInputAction::ValidateData(FDataValidationContext& InContext, const int InIndex, const FString& InInputActionsName) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	if(InputAction == nullptr)
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Input Action at index [%i] is null! \n"
			"Please set a valid Input Action class or delete this index entry in the %s Input Actions."), InIndex, *InInputActionsName));

		InContext.AddError(ErrorMessage);
	}
	
	if(!InputTag.IsValid())
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Input Tag at index [%i] is invalid! \n"
			"Please set a valid Input Tag or delete this index entry in the %s Input Actions."), InIndex, *InInputActionsName));

		InContext.AddError(ErrorMessage);
	}

	return Result;
}

EDataValidationResult UObsidianInputConfig::IsDataValid(FDataValidationContext& InContext) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(InContext), EDataValidationResult::Valid);

	unsigned int NativeIndex = 0;
	for(const FObsidianInputAction& Action : NativeInputActions)
	{
		Result =  CombineDataValidationResults(Result, Action.ValidateData(InContext, NativeIndex, FString("Native")));
		NativeIndex++;
	}
	
	unsigned int AbilityIndex = 0;
	for(const FObsidianInputAction& Action : AbilityInputActions)
	{
		Result =  CombineDataValidationResults(Result, Action.ValidateData(InContext, AbilityIndex, FString("Ability")));
		AbilityIndex++;
	}

	return Result;
}

#endif // ~ With Editor






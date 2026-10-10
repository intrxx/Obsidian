// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "ObsidianInputConfig.generated.h"

class UInputAction;

/**
 *	Struct used to map an input action to a gameplay tag that is used for input.
 */
USTRUCT(Blueprintable)
struct FObsidianInputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Input"))
	FGameplayTag InputTag;

#if WITH_EDITOR
	EDataValidationResult ValidateData(FDataValidationContext& InContext, const int InIndex, const FString& InInputActionsName) const;
#endif
};

/**
 * 
 */
UCLASS(BlueprintType, Const, meta = (DisplayName = "Obsidian Input Config", ShortToolTip = "Data asset used to hold Input Actions that corespond to Input Tag"))
class OBSIDIAN_API UObsidianInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UObsidianInputConfig(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|Input")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InInputTag, bool bInLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Input")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InInputTag, bool bInLogNotFound = true) const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& InContext) const override;
#endif
	
public:
	/** List of input actions used by the owner.  These input actions are mapped to a gameplay tag and must be manually bound. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FObsidianInputAction> NativeInputActions;

	/** List of input actions used by the owner.  These input actions are mapped to a gameplay tag and are automatically bound to abilities with matching input tags. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FObsidianInputAction> AbilityInputActions;
};

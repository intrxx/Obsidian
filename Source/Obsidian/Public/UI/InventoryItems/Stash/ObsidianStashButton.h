// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "UI/Components/ObsidianButtonBase.h"

#include "ObsidianStashButton.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStashTabButtonPressedSignature, const FGameplayTag& CorrespondingStashTag);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianStashButton : public UObsidianButtonBase
{
	GENERATED_BODY()

public:
	void InitializeStashButton(const FGameplayTag& InStashTag, const FText& InStashTabName);

	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~ End UUserWidget Interface

	FOnStashTabButtonPressedSignature OnStashTabButtonPressedDelegate;

protected:
	void OnStashButtonClicked();

	UPROPERTY()
	FGameplayTag CorrespondingStashTag = FGameplayTag::EmptyTag;
};

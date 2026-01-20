// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project

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
	void InitializeStashButton(const FGameplayTag& StashTag, const FText& StashTabName);

public:
	FOnStashTabButtonPressedSignature OnStashTabButtonPressedDelegate;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void OnStashButtonClicked();
	
protected:
	UPROPERTY()
	FGameplayTag CorrespondingStashTag = FGameplayTag::EmptyTag;
};

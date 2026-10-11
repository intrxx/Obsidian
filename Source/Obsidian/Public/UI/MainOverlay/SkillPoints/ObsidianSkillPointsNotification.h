// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "UI/ObsidianWidgetBase.h"

#include "ObsidianSkillPointsNotification.generated.h"

class UButton;
class UCommonTextBlock;

DECLARE_MULTICAST_DELEGATE(FOnSkillPointsNotificationPressedSignature);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianSkillPointsNotification : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void SetSkillPointsCount(const float InNewPoints) const;

	FOnSkillPointsNotificationPressedSignature OnSkillPointsNotificationPressedDelegate;

protected:
	UFUNCTION()
	void OnPressed();

	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	//~ End UUserWidget Interface

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> SkillPointsCounter_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> OpenTree_Button;
};
